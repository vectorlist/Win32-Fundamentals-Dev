#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <vector>
#include <Log.h>
#include <strsafe.h>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define WC_WINDOW	"WC_WINDOW"
#define WC_CHILD	"WC_CHILD"
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
BOOL isFullCreen = false;
HWND mainHwnd;
HWND childHwnd;
#define BUTTON_FULLWINDOW	0
#define BUTTON_MAXIMIZE		1
#define BUTTON_RESTORE		2
HWND button[3];

typedef struct tagDisplayInfo
{

}DisplayInfo;


UINT GetChildsNum(HWND hwnd) {
	UINT num = 0;
	HWND child = GetWindow(hwnd, GW_CHILD);
	if (!child) return num;

	HWND nextChild = child;
	while (nextChild != NULL)
	{
		nextChild = GetNextWindow(nextChild, GW_HWNDNEXT);
		num++;
	}
	return num;
}

BOOL GetChildHwnds(HWND hwnd, UINT len, HWND* dstHwnd) {
	HWND nextHwnd = GetWindow(hwnd, GW_CHILD);
	if (!nextHwnd) return FALSE;
	for (int i = 0; i < len; i++) {
		dstHwnd[i] = nextHwnd;
		nextHwnd = GetNextWindow(nextHwnd, GW_HWNDNEXT);
	}
	return TRUE;
}

void SetDisplayCapability(UINT width, UINT height) {
	DEVMODE dm{};
	dm.dmSize = sizeof(DEVMODE);
	for (int i = 0; EnumDisplaySettings(NULL, i, &dm); i++) {
		SIZE size{dm.dmPelsWidth, dm.dmPelsHeight };
		printf("Mode : %d, (%d x %d)\n", i, size.cx, size.cy);
		
		if (size.cx == width && size.cy == height) {
			printf("Found a Mode : %d, (%d x %d)\n", i, width, height);
			break;
		}
	}

}

void GetDisplayInfo() {
	SIZE size{};
	size.cx = ::GetSystemMetrics(SM_CXSCREEN);
	size.cy = ::GetSystemMetrics(SM_CYSCREEN);
	UINT monitors = ::GetSystemMetrics(SM_CMONITORS);

}

void GetDisplayDevice() {
	DISPLAY_DEVICE device{};
	device.cb = sizeof(DISPLAY_DEVICE);
	EnumDisplayDevices(NULL, 0, &device, 0);

	TCHAR name[33];
	TCHAR lpDeviceName[129];
	HRESULT res = StringCchCopy(name, 33, device.DeviceName);
	printf("Graphic Device : %s\n", device.DeviceString);
	EnumDisplayDevices(name, 0, &device, 0);

	res = StringCchCopy(lpDeviceName, 129, device.DeviceString);
	printf("Display Divice : %s\n", lpDeviceName);
}

int main(int args, char* argv[])
{
	InitCommonControls();
	HINSTANCE hInst = GetModuleHandle(NULL);
	WNDCLASSEX wc{};
	wc.cbSize        = sizeof(wc);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hInstance     = hInst;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = WC_WINDOW;
	wc.style         = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc   = WndProc;
	GetClassInfoEx(wc.hInstance, wc.lpszClassName, &wc);
	RegisterClassEx(&wc);

	mainHwnd = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW,
		WS_POPUP | WS_VISIBLE, 500, 200, 700, 300, nullptr, 0, wc.hInstance, NULL);

	wc.lpszClassName = WC_CHILD;
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	RegisterClassEx(&wc);

	//childHwnd = CreateWindowEx(NULL, wc.lpszClassName, "Button",
	//	WS_CHILD | WS_VISIBLE, 170, 80, 200, 50, mainHwnd, 0, wc.hInstance, NULL);
	ShowWindow(mainHwnd, TRUE);
	//SendMessage(mainHwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
	
	button[0] = CreateWindowEx(NULL, WC_BUTTON, "full screen", WS_VISIBLE | WS_CHILD,
		30, 30, 120, 38, mainHwnd, (HMENU)0, hInst, NULL);

	button[1] = CreateWindowEx(NULL, WC_BUTTON, "restore", WS_VISIBLE | WS_CHILD,
		160, 30, 120, 38, mainHwnd, (HMENU)1, hInst, NULL);
	printf("button creating\n");
	button[2] = CreateWindowEx(NULL, WC_BUTTON, "full area", WS_VISIBLE | WS_CHILD,
		300, 30, 120, 38, mainHwnd, (HMENU)2, hInst, NULL);
	printf("button created\n");

	UINT num = GetChildsNum(mainHwnd);
	//HWND childs[2];
	std::vector<HWND> childs(num);
	GetChildHwnds(mainHwnd, num, childs.data());

	printf("childs %d\n", GetChildsNum(mainHwnd));

	//Method 1
	MONITORINFO info{ sizeof(MONITORINFO) };
	HMONITOR monitor = MonitorFromWindow(mainHwnd, MONITOR_DEFAULTTONEAREST);
	GetMonitorInfo(monitor, &info);
	if (info.dwFlags == MONITORINFOF_PRIMARY) {

	}
	RECT rc = info.rcWork;
	//method 2
	RECT workRc;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workRc, 0);
	
	//SetWindowPos(mainHwnd, HWND_NOTOPMOST, rc.left, rc.top,
	//	rc.width(), rc.height(),
	//	SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE);

	SetDisplayCapability(640,480);
	GetDisplayDevice();

	WINDOWINFO wndInfo{};
	//GetWindowInfo(mainHwnd, &wndInfo);

	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.lParam;
}




LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	RECT rc;
	TCHAR code[256];
	//printf("MSG : %s\n",LOG::WndMessage(msg));
	switch (msg)
	{
	case WM_CREATE: {
		break;
	}
	case WM_SYSCOMMAND: {
		printf("WM_SYSCOMMAND\n");
		switch (wp)
		{
		case SC_CLOSE: {
			break;
		}
		case SC_MAXIMIZE: {
			break;
		}
		case SC_MINIMIZE: {
			break;
		}
		case SC_RESTORE: {
			printf("WM_SYSCOMMAND : SC_RESTORE\n");
			break;
		}
		}
		break;
	}
	case WM_PAINT: {
		break;
	}
	case WM_PARENTNOTIFY: {
		if (wp == WM_CREATE) {
			HWND lpHwnd = (HWND)lp;
			printf("created %d\n", 0);
		}
		return 0;
		break;
	}
	case WM_COMMAND: {
		HWND lpHwnd = (HWND)lp;
		switch (wp)
		{
		case 0: {
			printf("0 BUTTON\n");
			SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
			break;
		}
		case 3: {
			break;
		}
		}
		break;
	}
	case WM_SIZE: {
		break;
	}
	case WM_LBUTTONDOWN: {
		if (hwnd != childHwnd) break;
		printf("WM_LBUTTONDOWN\n");
		HWND parent = GetParent(hwnd);
		HWND taskbar = FindWindow("Shell_TrayWnd", NULL);
		//ShowWindow(taskbar, SW_SHOW);
		//DISP_CHANGE_SUCCESSFUL       0
		//DISP_CHANGE_RESTART          1
		//DISP_CHANGE_FAILED          -1
		//DISP_CHANGE_BADMODE         -2
		//DISP_CHANGE_NOTUPDATED      -3
		//DISP_CHANGE_BADFLAGS        -4
		//DISP_CHANGE_BADPARAM        -5
		//DISP_CHANGE_BADDUALVIEW     -6
		DEVMODE mode{};
		mode.dmSize = sizeof(DEVMODE);
		mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		mode.dmBitsPerPel = 16;
		mode.dmPelsWidth = 640;
		mode.dmPelsHeight = 480;
		LONG res = ChangeDisplaySettings(&mode, CDS_FULLSCREEN);
		//printf("ChangeDisplaySettings %d\n", res);
		//
		//if (!isFullCreen) {
		//	SendMessage(parent, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		//	isFullCreen = TRUE;
		//}
		//else {
		//	isFullCreen = FALSE;
		//	SendMessage(parent, WM_SYSCOMMAND, SC_RESTORE, 0);
		//}
		//
		//break;
	}
	case WM_KEYDOWN: {
		if(hwnd != mainHwnd)break;
		if (wp == VK_ESCAPE) {
			printf("VK_ESCAFE\n");
			if (IsZoomed(hwnd)) {
				OutputDebugString("fulls");
				//SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
			}
		}
		break;
	}
	case WM_GETMINMAXINFO: {
		//LPMINMAXINFO info = (MINMAXINFO*)lp;
		//info->ptMaxSize = {400,500};
		//info->ptMaxTrackSize.x = 600;
		//info->ptMaxTrackSize.y = 400;
		//break;
	}

	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
