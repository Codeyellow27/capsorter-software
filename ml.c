// Written by Jonah Meggs, summer 2019-20
// Image processing program for a cap sorter.
// Uses OpenCV V2
// Any bugs or inquiries please email jmeggs@outlook.com.au

#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <windows.h>
#include <sys/types.h>
#include <sys/timeb.h>

#define X 320               // x coordinate for image analysis. Don't change unless you also change the offset calculated in analyse()
#define SEND_COLOUR 1       // Used to tell capsorter.exe that we are sending a colour
#define CAMERA_READY 2      // Used to tell capsorter.exe that the calibration is complete

#define SCREEN 0            // Enable live stream of camera, with points indicating Y1 and Y2. No info is sent to capsorter.exe. Use for debugging (run ml.exe)

//     ** WARNING **    //

/* Only enable DEBUG for short periods of time, 
   there is a lot of debugging information saved 
   to a file (ImageProcessingLog.txt) and the 
   file will grow large very quickly. It is 
   recommended to only debug when SCREEN is also 
   enabled. The debug file is replaced every
   time ml.exe is run.
*/
#define DEBUG 0             // Enable debugging statements

#define CAMERA_ID 1                                // Camera number, change if ml.exe exits with "Failed to open camera." (0 is usually built in webcam, so try increasing)

/* CALIBRATION: involves finding a black line and setting 
   y coordinates on and just below the line to enable cap 
   recognition. Works by starting at CALIBRATE_Y_START_POSITION 
   and moving down until the intensity moves below a certain 
   threshold (controlled by CALIBRATE_DARK_OFFSET). The width 
   of the dark line is determined by moving down until the 
   intensity moves above a threshold controlled by CALIBRATE_WHITE_OFFSET
   and Y1 is set to the middle of the line. If an end to the 
   line is not found (CALIBRATE_Y_LIMIT_POSITION is exceeded), 
   the calibration procedure fails and is repeated on the next 
   frame. Y2 is set to an offset (CALIBRATE_Y2_OFFSET) below Y1.
    ** Note - intensities are red intensities (hence the number 2 is everywhere)
*/
#define CALIBRATE_Y_START_POSITION 240              // The Y coordinate pixel that calibration starts from.
#define CALIBRATE_Y_LIMIT_POSITION 420              // The Y coordinate that indicatesz calibration failed
#define CALIBRATTE_WHITE_OFFSET 50                  // Increase for the high intensity threshold to decrease
#define CALIBRATE_DARK_OFFSET 40                    // Increase for the low intensity threshold to decrease
#define CALIBRATE_Y2_OFFSET 5                       // Increase to move Y2 further away from Y1

/* IDENTIFICATION: involves monitoring the intensity difference 
   between the 2 points derived during calibration (one is on 
   the black line, the other is on the white surface below the 
   line). A cap is identified when the intensity difference of 
   these 2 points is less than IDENTIFICATION_INTENSITY_DIFF.
    ** Note - intensities are red intensities
*/
#define IDENTIFICATION_INTENSITY_DIFF 40

/* ANALYSIS: involves moving left and right from Y1 and Y2 when 
   a cap is identified, until the intensity difference condition 
   (less than ANALYSIS_INTENSITY_DIFF) is no longer met. The 
   colour is then determined based on an average of pixels. The 
   centre of the cap is also determined. The length of the cap 
   is truncated to MAX_CAP_PIXEL_LENGTH, and if shorter than 
   MIN_CAP_PIXEL_LENGTH the analysis fails. To save time the 
   pixel jumps left and right by X_PIXEL_JUMP.
    ** Note - intensities are red intensities
*/
#define ANALYSIS_INTENSITY_DIFF 40
#define MAX_CAP_PIXEL_LENGTH 200
#define MIN_CAP_PIXEL_LENGTH 10
#define X_PIXEL_JUMP  5


int Ya = 0, Yb = 0;

int Y1 = 0;
int Y2 = 0;
FILE *fp;

struct _timeb start;

int isCap(IplImage* image);
void analyse(IplImage* image);
void whichColour(int red, int green, int blue, double offset);
int calibrate(IplImage* image);
void sendColour(COLORREF colour, int id, double offset);

int main(void) {
    if (DEBUG) {
        fp = fopen("ImageProcessingLog.txt","w");
    }
    int found = 0;
    IplImage *im;
    CvCapture *cam = cvCaptureFromCAM(CAMERA_ID);
    if (!cam) {
        fprintf (fp, "Failed to open camera.\n");
        return (-1);
    }
    if (SCREEN) cvNamedWindow("stream",CV_WINDOW_AUTOSIZE);
    CvPoint pt1; //top line
    CvPoint pt2;
    if (DEBUG) fprintf(fp, "Calibrating...\n");
    while(1) {
        im = cvQueryFrame(cam);
        if (im == NULL) {
            printf ("Failed to grab frame.\n");
            //return (-1);
        }
        if (!found) {
            found = calibrate(im);
            if (found) {
                pt1 = cvPoint(X, Y1); //top line
                pt2 = cvPoint(X, Y2);
                sendColour(0, CAMERA_READY, 0);
            }
            continue;
        }
        if (isCap(im)) {
            analyse(im);
        }
        if (SCREEN) {
            cvRectangle(im, pt1, pt1, CV_RGB(0,0xff,0), 1,8,0);
            cvRectangle(im, pt2, pt2, CV_RGB(0xff,0,0), 1,8,0);
            cvShowImage("stream",im);
            if(cvWaitKey(5) == 27) break;
        }
    }
    cvSaveImage("test.bmp", im,0);
    cvReleaseCapture(&cam);
    cvDestroyAllWindows();
    if (DEBUG) fclose(fp);
    return 0;
}

int calibrate(IplImage* image) {
    static int delay = 0;
    if (delay < 50 || delay > 100) {
        delay++;
        return 0;
    } else delay++;
    uint8_t* pixelPtr = (uint8_t*)image->imageData;
    int cn = image->nChannels, i = CALIBRATE_Y_START_POSITION, start = 0, finish = 0, j = X, far1 = CALIBRATE_Y_LIMIT_POSITION, hi = pixelPtr[i*image->width*cn + j*cn + 2]-CALIBRATTE_WHITE_OFFSET, lo = hi - CALIBRATE_DARK_OFFSET;
    while (Y1 == 0) {
        if (i > far1) {
            if (DEBUG) fprintf(fp, "\n");
            return 0;
        }
        if (start == 0 && pixelPtr[i*image->width*cn + j*cn + 2] < lo) start = i;
        else if (start != 0 && finish == 0 && pixelPtr[i*image->width*cn + j*cn + 2] > hi) {
            finish = i;
            Y1 = (finish-start)/2+start-1;
        }
        if (DEBUG) fprintf(fp, "i = %d, red_val=%d\n", i, pixelPtr[i*image->width*cn + j*cn + 2]);
        i++;
    }
    Y2 = finish+CALIBRATE_Y2_OFFSET;
    if (DEBUG) fprintf(fp, "Finished calibration!\n\n");
    return 1;
}



int isCap(IplImage* image) {
    uint8_t* pixelPtr = (uint8_t*)image->imageData;
    int cn = image->nChannels, red1, red2;
    
    red1 = pixelPtr[Y1*image->width*cn + X*cn + 2];
    red2 = pixelPtr[Y2*image->width*cn + X*cn + 2];
    if (DEBUG) fprintf(fp, "Cap identification, intensity difference is %d\n", abs(red1-red2));
    if (abs(red1-red2) <= IDENTIFICATION_INTENSITY_DIFF)  {
        Ya = Y1;
        Yb = Y2;
        _ftime(&start);
        if (DEBUG) fprintf(fp, "Cap identified! Analysing colour...\n");
        return 1;
    }
    return 0;
}

void analyse(IplImage* image) {
    int red = 0, green = 0, blue = 0, cap = 1, red1, red2, diff = 10, diff2 = 10;
    double offset = 0;
    uint8_t* pixelPtr = (uint8_t*)image->imageData;
    int cn = image->nChannels;
    static int count = 0;
    //find rectangle
    while(cap) {
        red1 = pixelPtr[Ya*image->width*cn + (X+diff)*cn + 2];
        red2 = pixelPtr[Yb*image->width*cn + (X+diff)*cn + 2];
        if (abs(red1-red2) > ANALYSIS_INTENSITY_DIFF) {
            cap = 0;
            diff -= X_PIXEL_JUMP;
        } else diff += X_PIXEL_JUMP;
    }
    cap = 1;
    while(cap) {
        red1 = pixelPtr[Ya*image->width*cn + (X-diff2)*cn + 2];
        red2 = pixelPtr[Yb*image->width*cn + (X-diff2)*cn + 2];
        if (abs(red1-red2) > ANALYSIS_INTENSITY_DIFF) {
            cap = 0;
            diff2 -= X_PIXEL_JUMP;
        } else diff2 += X_PIXEL_JUMP;
    }
    diff += diff2;
    if (diff < MIN_CAP_PIXEL_LENGTH) return;
    if (diff > MAX_CAP_PIXEL_LENGTH) diff = MAX_CAP_PIXEL_LENGTH;
    //analyse rectangle
    offset = (diff2-diff/2)*0.3;
    if (DEBUG) fprintf(fp, "Cap parameters: diff2 (left offset from centre of image): %d, diff (length of cap (pixels)): %d\n", diff2, diff);
    
    int left = X-diff2, right = X+(diff-diff2);
    for(int j = left; j <= right; j++) {
        red += pixelPtr[Yb*image->width*cn + j*cn + 2];
        green += pixelPtr[Yb*image->width*cn + j*cn + 1];
        blue += pixelPtr[Yb*image->width*cn + j*cn + 0];
    }
    CvPoint pt1 = cvPoint(left,Yb); 
    CvPoint pt2 = cvPoint(right, Yb);
    cvRectangle(image, pt1, pt2, CV_RGB(0,0,0), 1,8,0);
    cvSaveImage("test.bmp", image, 0);
    int n = (diff+1);
    red = red/n;
    green = green/n;
    blue = blue/n;
    whichColour(red, green, blue, offset);
    return;
}

void whichColour(int red, int green, int blue, double offset) {
    sendColour(RGB(red, green, blue), SEND_COLOUR, offset);
    if (DEBUG) fprintf(fp, "Colour identified is r=%d g=%d b=%d\n\n", red, green, blue);
}

typedef struct tagMYREC
{
   COLORREF colour;
   double offset;
   unsigned short int startTime;
} MYREC;
COPYDATASTRUCT MyCDS;
MYREC MyRec;



void sendColour(COLORREF colour, int id, double offset) {
    HWND hwDispatch;
    MyRec.colour = colour;
    MyCDS.dwData = id;          // function identifier
    MyCDS.cbData = sizeof( MyRec );  // size of data
    MyCDS.lpData = &MyRec;           // data structure
    hwDispatch = FindWindow("myWindowClass", "CapSorter");
    MyRec.offset = offset;
    MyRec.startTime = start.millitm;
    if(hwDispatch != NULL) SendMessage(hwDispatch, WM_COPYDATA, (WPARAM)(HWND)NULL, (LPARAM)(LPVOID)&MyCDS);
    else if (!SCREEN) MessageBox( NULL, "Can't send WM_COPYDATA", "MyApp", MB_OK );
}