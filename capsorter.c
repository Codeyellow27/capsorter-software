//Yoooo

#include "resource.h" 

const char g_szClassName[] = "myWindowClass";

LRESULT CALLBACK JBinProc(HWND hmsg, UINT msg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    int id = GetDlgCtrlID(hmsg), index;
    HDC hdc;
    //printf("message is %d\n", msg);
    switch(msg) {
        case WM_LBUTTONDOWN:
            if (id >= ID_BINBUTTON && id < ID_BINBUTTON+MAXCOLOURS && shade) {
                DeleteBin(id-ID_BINBUTTON);
                ShowWindow(hbbin[numColours], SW_HIDE);
                ShowWindow(htbin[numColours], SW_HIDE);
                for(int i = 0; i < numColours; i++) {
                    InvalidateRect(hbbin[i], NULL, 0);
                    SendMessage(hbbin[i], WM_PAINT, 0, 0);
                }
                MoveWindow(hbNew, (4+numColours%4)*WinW/8+(WinW/8-cap_w)/2, (numColours/4)*WinH/5+5, cap_w, cap_h, 1);
            } else if (id >= ID_BINBUTTON && id < ID_BINBUTTON+MAXCOLOURS && pMyCDS != NULL) {
            	updatePast();
            	index = id-ID_BINBUTTON;
            	//printf("received, data is %d %d %d\n", GetRValue(received), GetGValue(received), GetBValue(received));
            	col[index].r = (col[index].r*col[index].n+GetRValue(received))/(col[index].n+1);
            	col[index].g = (col[index].g*col[index].n+GetGValue(received))/(col[index].n+1);
            	col[index].b = (col[index].b*col[index].n+GetBValue(received))/(col[index].n+1);
            	col[index].n++;
            	col[index].rgb = RGB(col[index].r, col[index].g, col[index].b);
            	//printf("%d %d %d\n", col[index].r, col[index].g, col[index].b);
            	pMyCDS = NULL;
            	InvalidateRect(hbbin[index], NULL, 0);
				SendMessage(hbbin[index], WM_PAINT, 0, 0);
            } else  DefWindowProc(hmsg, msg, wParam, lParam);
            break;
        case WM_PAINT:
            hdc = BeginPaint (hmsg, &ps);
            switch (id) {
                case ID_READCOLOUR:
                	if (pMyCDS != NULL) {
                		paintBin(hdc, hmsg, (COLORREF) ((MYREC *)(pMyCDS->lpData))->colour, hfLarge);
                	}else {
                		SetWindowText(hbReadColour, "No data");
                		paintBin(hdc, hmsg, RGB(255,255,255), hfLarge);
                		SetWindowText(hbReadColour, "");
                	}
                	break;
                default:
                    paintBin(hdc, hmsg, col[id-ID_BINBUTTON].rgb, hfLarge);
            }
            EndPaint (hmsg, &ps);
            break;
        default:
            return DefWindowProc(hmsg, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK JTextProc(HWND hmsg, UINT msg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;
    switch(msg) {
        case WM_PAINT:
            hdc = BeginPaint (hmsg, &ps);
            paintText(hdc, hmsg, RGB(255,255,255), hfLarge);
            EndPaint (hmsg, &ps);
            break;
        default:
            return DefWindowProc(hmsg, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK JThemeProc(HWND hmsg, UINT msg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    int id = GetDlgCtrlID(hmsg);
    HDC hdc, hdcMem;
    HBITMAP hbmOld;
    HANDLE hIcon;
    BITMAP bm;
    RECT rc;
    switch(msg) {
    	case WM_LBUTTONDOWN:
            switch(id) {
            	case ID_UNDO:
            		restorePast();
            		break;
            	case ID_ADD:
            		shade ^= 1;
            		for(int i = 0; i < numColours; i++) {
            			InvalidateRect(hbbin[i], NULL, 0);
						SendMessage(hbbin[i], WM_PAINT, 0, 0);
            		}
            		if (numColours < MAXCOLOURS && shade) {
            			MoveWindow(hbNew, (4+numColours%4)*WinW/8+(WinW/8-cap_w)/2, (numColours/4)*WinH/5+5, cap_w, cap_h, 1);
            			ShowWindow(hbNew, SW_SHOW);
            		} else  ShowWindow(hbNew, SW_HIDE);
            		break;
            	case ID_SKIP:

            		break;
            	case ID_CLOSE:
            		saveData();
            		DestroyWindow(hwnd);
            		break;
            	case ID_NEW:
            		if (pMyCDS == NULL) {
            			MessageBox(NULL, "No colour data available", "Error!", MB_ICONEXCLAMATION | MB_OK);
            			break;
            		} else {
            			updatePast();
            			col[numColours].r = GetRValue(received);
		            	col[numColours].g = GetGValue(received);
		            	col[numColours].b = GetBValue(received);
		            	col[numColours].n++;
		            	col[numColours].rgb = received;
		            	numColours++;
		            	//printf("%d %d %d\n", col[numColours].r, col[numColours].g, col[numColours].b);
		            	pMyCDS = NULL;
		            	shade ^= 1;
		            	ShowWindow(hbNew, SW_HIDE);
	            		for(int i = 0; i < numColours-1; i++) {
	            			InvalidateRect(hbbin[i], NULL, 0);
							SendMessage(hbbin[i], WM_PAINT, 0, 0);
	            		}
	            		ShowWindow(hbbin[numColours-1], SW_SHOW);
	            		ShowWindow(htbin[numColours-1], SW_SHOW);
            		}
            		break;
            }
            break;
        case WM_PAINT:
        	switch(id) {
        		case ID_UNDO:
					hIcon = LoadImage(NULL, "undo.bmp", IMAGE_BITMAP, WinW/20,WinW/20,LR_LOADFROMFILE);
					if (hIcon == NULL) break;
			        hdc = BeginPaint(hmsg, &ps);
			        hdcMem = CreateCompatibleDC(hdc);
			        hbmOld = SelectObject(hdcMem, hIcon);
			        GetObject(hIcon, sizeof(bm), &bm);
			        BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
			        SelectObject(hdcMem, hbmOld);
			        DeleteDC(hdcMem);
			        DeleteObject(hIcon);
			        EndPaint(hmsg, &ps);
			        break;
        		case ID_ADD:
        			hIcon = LoadImage(NULL, "add-remove.bmp", IMAGE_BITMAP, WinW/20,WinW/20,LR_LOADFROMFILE);
					if (hIcon == NULL) break;
			        hdc = BeginPaint(hmsg, &ps);
			        hdcMem = CreateCompatibleDC(hdc);
			        hbmOld = SelectObject(hdcMem, hIcon);
			        GetObject(hIcon, sizeof(bm), &bm);
			        BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
			        SelectObject(hdcMem, hbmOld);
			        DeleteDC(hdcMem);
			        DeleteObject(hIcon);
			        EndPaint(hmsg, &ps);
			        break;
        		case ID_SKIP:
        			hIcon = LoadImage(NULL, "skip.bmp", IMAGE_BITMAP, WinW/20,WinW/20,LR_LOADFROMFILE);
					if (hIcon == NULL) break;
			        hdc = BeginPaint(hmsg, &ps);
			        hdcMem = CreateCompatibleDC(hdc);
			        hbmOld = SelectObject(hdcMem, hIcon);
			        GetObject(hIcon, sizeof(bm), &bm);
			        BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
			        SelectObject(hdcMem, hbmOld);
			        DeleteDC(hdcMem);
			        DeleteObject(hIcon);
			        EndPaint(hmsg, &ps);
			        break;
        		case ID_CLOSE:
        			hIcon = LoadImage(NULL, "close.bmp", IMAGE_BITMAP, WinW/20,WinW/20,LR_LOADFROMFILE);
					if (hIcon == NULL) break;
			        hdc = BeginPaint(hmsg, &ps);
			        hdcMem = CreateCompatibleDC(hdc);
			        hbmOld = SelectObject(hdcMem, hIcon);
			        GetObject(hIcon, sizeof(bm), &bm);
			        BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
			        SelectObject(hdcMem, hbmOld);
			        DeleteDC(hdcMem);
			        DeleteObject(hIcon);
			        EndPaint(hmsg, &ps);
			        break;
			    case ID_NEW:
			    	GetClientRect(hmsg, &rc);
        			hIcon = LoadImage(NULL, "new.bmp", IMAGE_BITMAP, cap_h, cap_h,LR_LOADFROMFILE);
					if (hIcon == NULL) break;
			        hdc = BeginPaint(hmsg, &ps);
			        FillRect(hdc, &rc, GetStockObject(WHITE_BRUSH));
			        hdcMem = CreateCompatibleDC(hdc);
			        hbmOld = SelectObject(hdcMem, hIcon);
			        GetObject(hIcon, sizeof(bm), &bm);
			        BitBlt(hdc, (cap_w-cap_h)/2, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
			        SelectObject(hdcMem, hbmOld);
			        DeleteDC(hdcMem);
			        DeleteObject(hIcon);
			        EndPaint(hmsg, &ps);
			        break;
        	}
            hdc = BeginPaint (hmsg, &ps);
            //paintText(hdc, hmsg, RGB(255,255,255), hfLarge);
            EndPaint (hmsg, &ps);
            break;
        default:
            return DefWindowProc(hmsg, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	char buf[20];
	RECT client;
	HANDLE hImage;
	DWORD exitCode;
    int bin;
    double offset;
    unsigned short int start;
	switch(Message)
	{
		case WM_COPYDATA:
			pMyCDS = (PCOPYDATASTRUCT) lParam;
			if (pMyCDS->dwData == MYACK) {
                printf("ready\n");
				InvalidateRect(htCalibrate, NULL, 0);
				SetWindowText(htCalibrate, "Camera Ready");
				break;
			}
			received = (COLORREF) ((MYREC *)(pMyCDS->lpData))->colour;
            offset = (double) ((MYREC *)(pMyCDS->lpData))->offset;
            start = (unsigned short int) ((MYREC *)(pMyCDS->lpData))->startTime;
            bin = guess(received);
            sendUSBMessage(bin, offset, start);
			InvalidateRect(hwnd, NULL, 0);
			SendMessage(hwnd, WM_PAINT, 0, 0);
			sprintf(buf, "Guess: Bin %d\n", bin);
			SetWindowText(hbGuess, buf);
			InvalidateRect(hbReadColour, NULL, 0);
			SendMessage(hbReadColour, WM_PAINT, 0, 0);
			//printf("received\n");
			//printf("received, data is %d %d %d\n", GetRValue(received), GetGValue(received), GetBValue(received));
			break;
		case WM_SIZE:
			//printf("resized\n");
			GetClientRect(hwnd, &client);
			HDC h = GetDC(hwnd);
			FillRect(h, &client, GetStockObject(WHITE_BRUSH));
			ReleaseDC(hwnd, h);
			WinW = client.right-client.left;
    		WinH = client.bottom-client.top;
		    if ((double)WinW/(10*(WinH/5-15-WinH/20)) < 1.4945) { //width restricted
		    	cap_w = 3*WinW/40;
		    	cap_h = cap_w/1.4945;
		    } else { //height restricted
		    	cap_h = 3*(WinH/5-WinH/20)/4;
		    	cap_w = cap_h*1.4945;
		    }
    		InvalidateRect(hwnd, NULL, 0);
    		SendMessage(hwnd, WM_PAINT, 0, 0);
    		for (int i = 0; i < numColours; i++) {
    			MoveWindow(hbbin[i], (4+i%4)*WinW/8+(WinW/8-cap_w)/2, (i/4)*WinH/5+5, cap_w, cap_h, 1);
    			MoveWindow(htbin[i], (4+i%4)*WinW/8, (i/4)*WinH/5+5+cap_h, WinW/8, WinH/20-5, 1);
    		}
    		MoveWindow(hbReadColour, WinW/8, 3*WinH/4, cap_w*1.5, cap_h*1.5, 1);
    		MoveWindow(hbGuess, 2*WinW/7, 3*WinH/4, WinW/6, WinH/10, 1);
    		break;
		case WM_PAINT:
			//if (hImage != NULL) DeleteObject(hImage);
			hImage = LoadImage(NULL, "test.bmp", IMAGE_BITMAP, 3*WinW/8,9*WinW/32,LR_LOADFROMFILE);
			if (hImage == NULL) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                EndPaint(hwnd, &ps);
            } else {
                BITMAP bm;
                HPEN hPen = CreatePen(PS_SOLID, 3, RGB(20,20,20));
                HGDIOBJ hBrushOriginal, hPenOriginal;
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                HDC hdcMem = CreateCompatibleDC(hdc);
                HBITMAP hbmOld = SelectObject(hdcMem, hImage);
                GetObject(hImage, sizeof(bm), &bm);
                BitBlt(hdc, WinW/10, WinH/12, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
                hBrushOriginal = SelectObject(hdc, GetStockObject(NULL_BRUSH));
                hPenOriginal = SelectObject(hdc, hPen);
                Rectangle(hdc,WinW/10,WinH/12,WinW/10+bm.bmWidth, WinH/12+bm.bmHeight);
                SelectObject(hdcMem, hbmOld);
                SelectObject(hdc, hBrushOriginal);
                SelectObject(hdc, hPenOriginal);
                DeleteDC(hdcMem);
                DeleteObject(hPen);
                DeleteObject(hImage);
                EndPaint(hwnd, &ps);
            }
			break;
		case WM_CLOSE:
			saveData();
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
			GetExitCodeProcess(ret->hProcess, &exitCode);
			TerminateProcess(ret->hProcess, (UINT)exitCode);
			free(ret);
			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

typedef void (WINAPI *PGNSI)(PCWSTR);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	PGNSI pGNSI;
	pGNSI = (PGNSI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "AddDllDirectory");
	char cwd[1000];
	_getcwd(cwd, strlen(cwd));
	sprintf(cwd, "%s\\libusb\\libusb-win32-bin-1.2.6.0\\bin\\amd64");
	if (pGNSI != NULL) pGNSI((PCWSTR)cwd);
	else printf("Nope\n");
	printf("executed\n");
	WNDCLASSEX wc;
	MSG Msg;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	WinW = GetSystemMetrics(SM_CXFULLSCREEN);
    WinH = GetSystemMetrics(SM_CYFULLSCREEN);
    //printf("%d %d\n", WinW, WinH);
	hwnd = CreateWindowEx(
		0,
		g_szClassName,
		"CapSorter",
		WS_OVERLAPPEDWINDOW,
		0, 0, WinW, WinH,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	if (!initWindows()) {
        MessageBox(NULL, "Control Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    for (int i = 0; i < MAXCOLOURS; i++) {
		col[i].r = col[i].g = col[i].b = col[i].n = col[i].rgb = 0;
	}
	//_set_errno(0);
	//int ret = _spawnl(_P_WAIT, "ml.exe", "");
	//if (ret < 0) printf("%d, errno = %d\n", ret, errno);
    //system("ml");
    //printf("yeeeee\n");
    ret = malloc(sizeof(*ret));
    LPSTARTUPINFO info = malloc(sizeof(*info));
    GetStartupInfo(info);
    int retval;
    //printf("mm\n");
    retval = CreateProcess(NULL, "ml", NULL, NULL, 0, NORMAL_PRIORITY_CLASS, NULL, NULL, info, ret);
    if (retval == 0) printf("failed\n");
    //printf("here\n");
    loadData();
	ShowWindow(hwnd, SW_MAXIMIZE);
	ShowWindow(htCalibrate, SW_SHOW);
	UpdateWindow(hwnd);
	showBins(SW_SHOW);
	showIcons(SW_SHOW);
	L = malloc(sizeof(*L));
	L->n = 0;
	L->head = NULL;
	L->tail = NULL;
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

int initWindows(void) {
	WNDCLASSEX wcJBin;
    wcJBin.cbSize        = sizeof(WNDCLASSEX);
    wcJBin.style         = 0;
    wcJBin.lpfnWndProc   = JBinProc;
    wcJBin.cbClsExtra    = 0;
    wcJBin.cbWndExtra    = 0;
    wcJBin.hInstance     = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
    wcJBin.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcJBin.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcJBin.hbrBackground = CreateSolidBrush(RGB(128,128,128));
    wcJBin.lpszMenuName  = NULL;
    wcJBin.lpszClassName = "JBin";
    wcJBin.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
    if(!RegisterClassEx(&wcJBin)) {
        MessageBox(NULL, "Window Class (JBin) Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    WNDCLASSEX wcJText;
    wcJText.cbSize        = sizeof(WNDCLASSEX);
    wcJText.style         = 0;
    wcJText.lpfnWndProc   = JTextProc;
    wcJText.cbClsExtra    = 0;
    wcJText.cbWndExtra    = 0;
    wcJText.hInstance     = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
    wcJText.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcJText.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcJText.hbrBackground = CreateSolidBrush(RGB(128,128,128));
    wcJText.lpszMenuName  = NULL;
    wcJText.lpszClassName = "JTEXT";
    wcJText.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
    if(!RegisterClassEx(&wcJText)) {
        MessageBox(NULL, "Window Class (JTEXT) Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    WNDCLASSEX wcJTheme;
    wcJTheme.cbSize        = sizeof(WNDCLASSEX);
    wcJTheme.style         = 0;
    wcJTheme.lpfnWndProc   = JThemeProc;
    wcJTheme.cbClsExtra    = 0;
    wcJTheme.cbWndExtra    = 0;
    wcJTheme.hInstance     = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
    wcJTheme.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcJTheme.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcJTheme.hbrBackground = CreateSolidBrush(RGB(128,128,128));
    wcJTheme.lpszMenuName  = NULL;
    wcJTheme.lpszClassName = "JTHEME";
    wcJTheme.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
    if(!RegisterClassEx(&wcJTheme)) {
        MessageBox(NULL, "Window Class (JTHEME) Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    int i = 0;
    char buf[10];
    int cap_w, cap_h;
    if ((double)WinW/(10*(WinH/5-15-WinH/20)) < 1.4945) { //width restricted
    	cap_w = 3*WinW/40;
    	cap_h = cap_w/1.4945;
    } else { //height restricted
    	cap_h = 3*(WinH/5-WinH/20)/4;
    	cap_w = cap_h*1.4945;
    }
    while (i < MAXCOLOURS) {
    	sprintf(buf, "Bin%d", i+1);
    	htbin[i] = CreateWindow("JTEXT", buf, WS_TABSTOP|WS_CHILD, (4+i%4)*WinW/8, (i/4)*WinH/5+5+cap_h, WinW/8, WinH/20-5, hwnd, (LPVOID)ID_BINTEXT+i, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
    	hbbin[i] = CreateWindow("JBin", "", WS_TABSTOP|WS_CHILD, (4+i%4)*WinW/8+(WinW/8-cap_w)/2, (i/4)*WinH/5+5, cap_w, cap_h, hwnd, (LPVOID)ID_BINBUTTON+i, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
    	if (htbin[i] == NULL || hbbin[i] == NULL) return 0;
    	i++;
    }
    //printf("width %d height %d", WinW/10, WinH/5-15-WinH/20);
    hfLarge = CreateFont(WinH/22,0,0,0,FW_SEMIBOLD,FALSE, FALSE, FALSE, DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH,"Calibri Light");
	if (hfLarge == NULL) return 0;
	hbReadColour = CreateWindow("JBin", "", WS_TABSTOP|WS_CHILD, WinW/8, 3*WinH/4, cap_w*1.5, cap_h*1.5, hwnd, (LPVOID)ID_READCOLOUR, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
    if (hbReadColour == NULL) return 0;
    hbGuess = CreateWindow("JTEXT", "Guess: n/a", WS_TABSTOP|WS_CHILD, 2*WinW/7, 3*WinH/4, WinW/6, WinH/10, hwnd, (LPVOID)ID_READCOLOUR, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
    if (hbGuess == NULL) return 0;
    htCalibrate = CreateWindow("JTEXT", "Calibrating camera, please wait...", WS_TABSTOP|WS_CHILD, WinW/8, 41*WinH/64, WinW/3, WinH/10, hwnd, (LPVOID)ID_CALIBRATE, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
    if (htCalibrate == NULL) return 0;
    hiUndo = CreateWindow("JTHEME", "Undo", WS_TABSTOP|WS_CHILD, WinW/40, WinH/12, WinW/20, WinW/20, hwnd, (LPVOID)ID_UNDO, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
    if (hiUndo == NULL) return 0;
    hiAdd = CreateWindow("JTHEME", "Add/Remove", WS_TABSTOP|WS_CHILD, WinW/40, WinH/12+WinW/20+10, WinW/20, WinW/20, hwnd, (LPVOID)ID_ADD, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
    if (hiAdd == NULL) return 0;
    hiSkip = CreateWindow("JTHEME", "Skip", WS_TABSTOP|WS_CHILD, WinW/40, WinH/12+2*(WinW/20+10), WinW/20, WinW/20, hwnd, (LPVOID)ID_SKIP, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
    if (hiSkip == NULL) return 0;
    hiClose = CreateWindow("JTHEME", "Close", WS_TABSTOP|WS_CHILD, WinW/40, WinH/12+3*(WinW/20+10), WinW/20, WinW/20, hwnd, (LPVOID)ID_CLOSE, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
    if (hiClose == NULL) return 0;
    hbNew = CreateWindow("JTHEME", "", WS_TABSTOP|WS_CHILD, 0, 0, cap_w, cap_h, hwnd, (LPVOID)ID_NEW, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
    if (hbNew == NULL) return 0;
	return 1;
}

void paintBin(HDC hdc, HWND hp, COLORREF colour, HFONT hf) {
	//colour = RGB(0,0,255);
	HBRUSH hBrush;
	HPEN hPen, hPen2;
	if(!shade) hBrush = CreateSolidBrush(colour);
	else hBrush = CreateSolidBrush(RGB(GetRValue(colour)+(255-GetRValue(colour))/2, GetGValue(colour)+(255-GetGValue(colour))/2, GetBValue(colour)+(255-GetBValue(colour))/2));
	if (!shade) hPen = CreatePen(PS_SOLID, 2, RGB(20,20,20));
	else hPen = CreatePen(PS_SOLID, 2, RGB(128,128,128));
    RECT buttonRC;
    char text[MAXNAME];
    GetWindowText(hp, text, MAXNAME);
    HGDIOBJ hBrushOriginal, hPenOriginal;
    SIZE sz;
    GetClientRect(hp, &buttonRC);
    int height = buttonRC.bottom-buttonRC.top, width = buttonRC.right-buttonRC.left;
    hPenOriginal = SelectObject(hdc, GetStockObject(WHITE_PEN));
    Rectangle(hdc, buttonRC.left, buttonRC.top, buttonRC.right, buttonRC.bottom);
    hBrushOriginal = SelectObject(hdc, hBrush);
    SelectObject(hdc, hPen);
    //FillRect(hdc, &buttonRC, hBrush);
    //Ellipse(hdc, buttonRC.left, buttonRC.top, buttonRC.right, buttonRC.top+2*(buttonRC.bottom-buttonRC.top)/5);
    Chord(hdc, buttonRC.left-3*width/4, buttonRC.top, buttonRC.right+3*width/4, buttonRC.top+2*width, buttonRC.right, buttonRC.top+2*width/5, buttonRC.left, buttonRC.top+2*width/5);
    Chord(hdc, buttonRC.left-3*width/4, buttonRC.bottom-2*width, buttonRC.right+3*width/4, buttonRC.bottom, buttonRC.left, buttonRC.bottom-2*width/5, buttonRC.right, buttonRC.bottom-2*width/5);
    //Ellipse(hdc, buttonRC.left, buttonRC.bottom-2*height/5, buttonRC.right, buttonRC.bottom);
    SelectObject(hdc, GetStockObject(NULL_PEN));
    Rectangle(hdc, buttonRC.left, buttonRC.top+height/6, buttonRC.right, buttonRC.bottom-height/6);
    SelectObject(hdc, hPen);
    Arc(hdc, buttonRC.left-3*width/4, buttonRC.top+height/3-2*width, buttonRC.right+3*width/4, buttonRC.top+height/3, buttonRC.left, buttonRC.top+height/3, buttonRC.right, buttonRC.top+height/3);
    MoveToEx (hdc, buttonRC.left, buttonRC.top+height/7, NULL);
	LineTo (hdc, buttonRC.left, buttonRC.bottom-height/7);
	int dist = width/12;
	for (int i = 1; i < 13; i++) {
		MoveToEx (hdc, buttonRC.left+i*dist-2, buttonRC.top+height/3+8-(i-6.5)*(i-6.5)/3, NULL);
		LineTo (hdc, buttonRC.left+i*dist-2, buttonRC.bottom-10-(i-6.5)*(i-6.5)/3);
	}
	MoveToEx (hdc, buttonRC.right-1, buttonRC.top+height/7, NULL);
	LineTo (hdc, buttonRC.right-1, buttonRC.bottom-height/7);
	if (shade) {
		hPen2 = CreatePen(PS_SOLID, 5, RGB(30,30,30));
		SelectObject(hdc, hPen2);
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		Ellipse(hdc, buttonRC.left+width/2-height/3, buttonRC.top+height/6, buttonRC.left+width/2+height/3, buttonRC.bottom-height/6);
		POINT apt[2] = {buttonRC.left+width/2-height/6, buttonRC.top+2*height/6, buttonRC.left+width/2+height/6, buttonRC.top+4*height/6};
		POINT apt2[2] = {buttonRC.left+width/2+height/6, buttonRC.top+2*height/6, buttonRC.left+width/2-height/6, buttonRC.top+4*height/6};
		Polyline(hdc, apt, 2);
		Polyline(hdc, apt2, 2);
		SelectObject(hdc, hPen);
		DeleteObject(hPen2);
	}
    SelectObject(hdc, hBrushOriginal);
    SelectObject(hdc, hPenOriginal);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

void paintText(HDC hdc, HWND hp, COLORREF colour, HFONT hf) {
	HBRUSH hBrush = CreateSolidBrush(colour);
    RECT buttonRC;
    char text[MAXNAME];
    GetWindowText(hp, text, MAXNAME);
    HGDIOBJ hFontOriginal;
    SIZE sz;
    GetClientRect(hp, &buttonRC);
    int height = buttonRC.bottom-buttonRC.top, width = buttonRC.right-buttonRC.left;
    FillRect(hdc, &buttonRC, hBrush);
    hFontOriginal = SelectObject(hdc, hf);
    //SetBkColor(hdc, colour);
    GetTextExtentPoint32(hdc, text, strlen(text), &sz);
    TextOut(hdc, (buttonRC.right-sz.cx)/2, (buttonRC.bottom-sz.cy)/2, text, strlen(text));
    SelectObject(hdc, hFontOriginal);
    DeleteObject(hBrush);
}

void showBins(int id) {
	for (int i = 0; i < numColours; i++) {
		ShowWindow(htbin[i], id);
		ShowWindow(hbbin[i], id);
	}
	ShowWindow(hbReadColour, id);
	ShowWindow(hbGuess, id);
}

int guess(COLORREF colour) {
	int *dist = malloc(numColours*sizeof(int));
	int r = GetRValue(colour), g = GetGValue(colour), b = GetBValue(colour), smallest = -1, index = 0;
	for (int i = 0; i < numColours; i++) {
        dist[i] = abs(r-col[i].r) > abs(g-col[i].g) ? abs(r-col[i].r) : abs(g-col[i].g);
        dist[i] = dist[i] > abs(b-col[i].b) ? dist[i] : abs(b-col[i].b);
		//dist[i] = abs(r-col[i].r) + abs(g-col[i].g) + abs(b-col[i].b);
		if (smallest == -1 || smallest > dist[i]) {
			smallest = dist[i];
			index = i;
		}
	}
	return index+1;
	free(dist);
}

void showIcons(int id) {
	ShowWindow(hiUndo, id);
	ShowWindow(hiAdd, id);
	ShowWindow(hiSkip, id);
	ShowWindow(hiClose, id);
}

void saveData(void) {
	FILE *fp;
	fp = fopen("data.txt","w");
	int ret = fwrite(col, sizeof(cols), MAXCOLOURS, fp);
	fclose(fp);
    if (ret < MAXCOLOURS) printf("Did not save correctly, saved %d chunks\n", ret);
}

void loadData(void) {
	FILE *fp;
	fp = fopen("data.txt","r");
    if (fp == NULL) {
        printf("load failed\n");
        return;
    }
	int count = fread(col,sizeof(cols),MAXCOLOURS,fp);
	fclose(fp);
	while (col[numColours].n != 0) {
		numColours++;
	}
    //printf("count: %d\n", count);
}

void DeleteBin(int id) {
	updatePast();
	for (int i = id; i < numColours-1; i++) {
		col[i].r = col[i+1].r;
    	col[i].g = col[i+1].g;
    	col[i].b = col[i+1].b;
    	col[i].n = col[i+1].n;
    	col[i].rgb = col[i+1].rgb;
	}
	col[numColours-1].r = 0;
	col[numColours-1].g = 0;
	col[numColours-1].b = 0;
	col[numColours-1].n = 0;
	col[numColours-1].rgb = 0;
	numColours--;
}

void updatePast(void) {
	saveData();
	Node new = malloc(sizeof(*new));
	for (int i = 0; i < MAXCOLOURS; i++) {
		new->col[i].r = col[i].r;
    	new->col[i].g = col[i].g;
    	new->col[i].b = col[i].b;
    	new->col[i].n = col[i].n;
    	new->col[i].rgb = col[i].rgb;
	}
	new->numColours = numColours;
	new->next = L->head;
	new->prev = NULL;
	L->head = new;
	if (new->next != NULL) new->next->prev = new;
	if (L->n == 10) {
		L->tail = L->tail->prev;
		free(L->tail->next);
		L->tail->next = NULL;
	} else L->n++;
	if (L->n == 1) L->tail = L->head;
}

void restorePast(void) {
	if (L->n == 0) {
		MessageBox(NULL, "Cannot undo any further!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	for (int i = 0; i < MAXCOLOURS; i++) {
		col[i].r = L->head->col[i].r;
    	col[i].g = L->head->col[i].g;
    	col[i].b = L->head->col[i].b;
    	col[i].n = L->head->col[i].n;
    	col[i].rgb = L->head->col[i].rgb;
	}
	numColours = L->head->numColours;
	Node temp = L->head;
	L->head = L->head->next;
	free(temp);
	if (L->head != NULL) L->head->prev = NULL;
	L->n--;
	if (L->n == 0) L->tail = NULL;
	MoveWindow(hbNew, (4+numColours%4)*WinW/8+(WinW/8-cap_w)/2, (numColours/4)*WinH/5+5, cap_w, cap_h, 1);
	for(int i = 0; i < numColours; i++) {
		ShowWindow(hbbin[i], SW_SHOW);
		ShowWindow(htbin[i], SW_SHOW);
		InvalidateRect(hbbin[i], NULL, 0);
		SendMessage(hbbin[i], WM_PAINT, 0, 0);
		InvalidateRect(htbin[i], NULL, 0);
		SendMessage(htbin[i], WM_PAINT, 0, 0);
	}
	for (int i = numColours; i < MAXCOLOURS; i++) {
		ShowWindow(hbbin[i], SW_HIDE);
		ShowWindow(htbin[i], SW_HIDE);
	}
}

static int usbGetDescriptorString(usb_dev_handle *dev, int index, int langid, 
                                  char *buf, int buflen) {
    char buffer[256];
    int rval, i;

    // make standard request GET_DESCRIPTOR, type string and given index 
    // (e.g. dev->iProduct)
    rval = usb_control_msg(dev, 
        USB_TYPE_STANDARD | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 
        USB_REQ_GET_DESCRIPTOR, (USB_DT_STRING << 8) + index, langid, 
        buffer, sizeof(buffer), 1000);
        
    if(rval < 0) // error
        return rval;
    
    // rval should be bytes read, but buffer[0] contains the actual response size
    if((unsigned char)buffer[0] < rval)
        rval = (unsigned char)buffer[0]; // string is shorter than bytes read
    
    if(buffer[1] != USB_DT_STRING) // second byte is the data type
        return 0; // invalid return type
        
    // we're dealing with UTF-16LE here so actual chars is half of rval,
    // and index 0 doesn't count
    rval /= 2;
    
    // lossy conversion to ISO Latin1 
    for(i = 1; i < rval && i < buflen; i++) {
        if(buffer[2 * i + 1] == 0)
            buf[i-1] = buffer[2 * i];
        else
            buf[i-1] = '?'; // outside of ISO Latin1 range
    }
    buf[i-1] = 0;
    
    return i-1;
}

static usb_dev_handle * usbOpenDevice(int vendor, char *vendorName, 
                                      int product, char *productName) {
    struct usb_bus *bus;
    struct usb_device *dev;
    char devVendor[256], devProduct[256];
    
    usb_dev_handle * handle = NULL;
    
    usb_init();
    usb_find_busses();
    usb_find_devices();
    
    for(bus=usb_get_busses(); bus; bus=bus->next) {
        for(dev=bus->devices; dev; dev=dev->next) {         
            if(dev->descriptor.idVendor != vendor ||
               dev->descriptor.idProduct != product)
                continue;
                
            // we need to open the device in order to query strings 
            if(!(handle = usb_open(dev))) {
                fprintf(stderr, "Warning: cannot open USB device: %s\n",
                    usb_strerror());
                continue;
            }
            
            // get vendor name 
            if(usbGetDescriptorString(handle, dev->descriptor.iManufacturer, 0x0409, devVendor, sizeof(devVendor)) < 0) {
                fprintf(stderr, 
                    "Warning: cannot query manufacturer for device: %s\n", 
                    usb_strerror());
                usb_close(handle);
                continue;
            }
            ///printf("Vendor name: %s\n", devVendor);
            
            // get product name 
            if(usbGetDescriptorString(handle, dev->descriptor.iProduct, 
               0x0409, devProduct, sizeof(devVendor)) < 0) {
                fprintf(stderr, 
                    "Warning: cannot query product for device: %s\n", 
                    usb_strerror());
                usb_close(handle);
                continue;
            }
            //printf("Product name: %s\n", devProduct);
            if(strcmp(devVendor, vendorName) == 0 && 
               strcmp(devProduct, productName) == 0)
                return handle;
            else
                usb_close(handle);
        }
    }
    
    return NULL;
}

void sendUSBMessage(int bin, double offset, unsigned short int start) {
    usb_dev_handle *handle = NULL;
    int nBytes = 0;
    char buffer;
    struct _timeb end;
    handle = usbOpenDevice(0x16C0, "Jonah Meggs", 0x05DC, "CapSorter");
    
    if(handle == NULL) {
        fprintf(stderr, "Could not find USB device while transmitting bin %d!\n", bin);
        //exit(1);
        return;
    }
    _ftime(&end);
    int dur;
    if (start <= end.millitm) dur = 0.865*(end.millitm-start);
    else dur = 0.865*(end.millitm+1000-start);
    buffer = (offset+dur)/2;
    if (buffer < 0) buffer = 0;
    nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, (unsigned char)bin, 0, 0, &buffer, 1, 5000);
    //printf("Offset: %lf, , time: %d, Buffer: %d\n", offset, dur, buffer);
    
    //if(nBytes < 0) fprintf(stderr, "USB error: %s\n", usb_strerror());
        
    usb_close(handle);
    
}