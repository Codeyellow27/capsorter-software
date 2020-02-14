// ************ Globals ************
//

#include <stdio.h>
#include <windows.h>
#define MYDISPLAY 1
typedef struct tagMYREC
{
   COLORREF colour;
} MYREC;
COPYDATASTRUCT MyCDS;
MYREC MyRec;



int main(void) {
  HWND hwDispatch;
  int x;
  while(1) {
    scanf("%d", &x);
    MyRec.colour = RGB(0,255,0);
    MyCDS.dwData = MYDISPLAY;          // function identifier
    MyCDS.cbData = sizeof( MyRec );  // size of data
    MyCDS.lpData = &MyRec;           // data structure
    hwDispatch = FindWindow("myWindowClass", "CapSorter");
    if( hwDispatch != NULL )
      SendMessage(hwDispatch, WM_COPYDATA, (WPARAM)(HWND)NULL, (LPARAM)(LPVOID)&MyCDS);
    else
      MessageBox( NULL, "Can't send WM_COPYDATA", "MyApp", MB_OK );
  }
}