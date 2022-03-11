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

struct Window
{
	HWND hwnd;
	LPCSTR name;
	HBITMAP bitmap = nullptr;
};

Window window;
Window button;

void DrawFillRect(HDC dc, const RECT &rc, COLORREF clr) {
	::SetBkColor(dc, clr);
	::ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
}

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

	window.name = "main window";
	window.hwnd = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW, WS_POPUP | WS_VISIBLE, 
		500, 200, 700, 300, nullptr, 0, wc.hInstance, NULL);

	wc.lpszClassName =  WC_SCROLLBAR;
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	RegisterClassEx(&wc);

	button.name = "Button";
	button.hwnd = CreateWindowEx(NULL, wc.lpszClassName, "Button", 
		WS_CHILD | WS_VISIBLE, 170, 80, 200, 50,
		window.hwnd, 0, wc.hInstance, NULL);
	ShowWindow(window.hwnd, TRUE);
	
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
	case WM_CREATE: {
		break;
	}
	case WM_NOTIFY: {

		break;
	}

	case WM_PAINT: {
		if (hwnd != button.hwnd) break;
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		HDC memDc = CreateCompatibleDC(dc);
		//HBITMAP bitMap = CreateBitmap()
		HBRUSH br = CreateSolidBrush(RGB(100, 100, 0));
		DrawFillRect(dc, ps.rcPaint, RGB(100, 100, 25));
		DeleteObject(br);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_SIZE: {
		//if (hwnd != button.hwnd) break;
		SIZE size{ LOWORD(lp), HIWORD(lp) };
		printf("size %d %d", size.cx, size.cy);
		int g = 19;
		break;
	}

	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
