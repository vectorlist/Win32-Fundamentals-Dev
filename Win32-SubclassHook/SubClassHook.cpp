#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define WC_WINDOW	"WC_WINDOW"
#define WC_CHILD	"WC_CHILD"
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT WINAPI SubClassWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR id, DWORD_PTR data);
HWND mainHwnd;
HWND subclassControl;
HWND subclassHookControl;
HHOOK hHook;

int main(int args, char* argv[])
{
	InitCommonControls();

	WNDCLASSEX wc{};
	wc.cbSize = sizeof(wc);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = WC_WINDOW;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WndProc;
	GetClassInfoEx(wc.hInstance, wc.lpszClassName, &wc);
	RegisterClassEx(&wc);

	mainHwnd = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW, WS_POPUP | WS_VISIBLE, 600, 400, 700, 300, nullptr, 0, wc.hInstance, NULL);

	wc.lpszClassName = WC_BUTTON;
	if (!GetClassInfoEx(wc.hInstance, wc.lpszClassName, &wc)) {
		printf("%s is built-in class... set subclass", wc.lpszClassName);
	}
	//Basic SubClassing
	subclassControl = CreateWindowEx(NULL, wc.lpszClassName, "SubClass Button", WS_CHILD | WS_VISIBLE, 50, 120, 200, 60, mainHwnd, 0, wc.hInstance, NULL);
	SetWindowSubclass(subclassControl, SubClassWndProc, 0, 0);
	//SubClassing with HOOK
	subclassHookControl = CreateWindowEx(NULL, wc.lpszClassName, "SubClass Hook Button", WS_CHILD | WS_VISIBLE, 250, 120, 200, 60, mainHwnd, 0, wc.hInstance, NULL);
	SetWindowSubclass(subclassHookControl, SubClassWndProc, 0, 0);
	ShowWindow(mainHwnd, TRUE);
	
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

	switch (msg)
	{
	case WM_NCCREATE:
		GetWindowText(hwnd, code, 256);
		printf("%s [wm_create]", code);
		break;
	case WM_PAINT: {
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rc);
		HBRUSH br = (HBRUSH)GetStockObject(DC_BRUSH);
		SelectObject(dc,br);
		if (hwnd == subclassControl) {
			SetDCBrushColor(dc, RGB(120, 80, 0));
		}
		else {
			SetDCBrushColor(dc, RGB(60, 60, 60));
		}
		FillRect(dc, &rc, br);
		
		GetWindowText(hwnd, code, 256);
		SetBkMode(dc, TRANSPARENT);
		SetTextColor(dc, RGB(220, 220, 220));
		if(hwnd == subclassControl)
			DrawText(dc, code, strlen(code), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		EndPaint(hwnd, &ps);
		break;
	}
	}
	if (hwnd == subclassControl) {
		return DefSubclassProc(hwnd, msg, wp, lp);
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT WINAPI SubClassWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR id, DWORD_PTR data)
{
	//return DefSubclassProc(hwnd, msg, wp, lp);
	return WndProc(hwnd, msg, wp, lp);
}
