#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include  <usb.h>
#include <sys/types.h>
#include <sys/timeb.h>

#define IDR_MYMENU                      101
#define ID_FILE_EXIT                    40001
#define ID_DIALOG_SHOW                  40002
#define ID_DIALOG_HIDE                  40003

#define ID_UNDO 0
#define ID_ADD 1
#define ID_SKIP 2
#define ID_CLOSE 3
#define ID_NEW 4
#define ID_CALIBRATE 5
#define ID_BINTEXT 10
#define ID_BINBUTTON 30
#define ID_READCOLOUR 50

#define MAXNAME 40
#define MAXCOLOURS 20

typedef struct cols_t {
	int r, g, b;
	int n;
	COLORREF rgb;
} cols;

cols col[MAXCOLOURS];
typedef struct node *Node;
struct node {
	cols col[MAXCOLOURS];
	int numColours;
	Node next;
	Node prev;
};
typedef struct stored *Stored;
struct stored {
	Node head;
	Node tail;
	int n;
};

Stored L;

COLORREF received;

int WinW;
int WinH;
int numColours = 0;
int shade = 0;
int cap_w, cap_h;
HWND htbin[MAXCOLOURS];
HWND hbbin[MAXCOLOURS];
HWND hbReadColour, hbGuess, htCalibrate;
HWND hiUndo, hiAdd, hiSkip, hiClose, hbNew;
HFONT hfLarge;
HWND hwnd;

LPPROCESS_INFORMATION ret;


PCOPYDATASTRUCT pMyCDS = NULL;

void paintBin(HDC hdc, HWND hp, COLORREF colour, HFONT hf);
void paintText(HDC hdc, HWND hp, COLORREF colour, HFONT hf);
int initWindows(void);
void showBins(int id);
void showIcons(int id);
LRESULT CALLBACK JBinProc(HWND hmsg, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK JTextProc(HWND hmsg, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK JThemeProc(HWND hmsg, UINT msg, WPARAM wParam, LPARAM lParam);
int guess(COLORREF colour);
void saveData(void);
void loadData(void);
void DeleteBin(int id);
void updatePast(void);
void restorePast(void);

void sendUSBMessage(int bin, double offset, unsigned short int start);

#define MYDISPLAY 1
#define MYACK 2
typedef struct tagMYREC
{
  COLORREF colour;
  double offset;
   unsigned short int startTime;
} MYREC;