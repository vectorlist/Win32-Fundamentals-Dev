#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <vector>
#include <strsafe.h>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <../Common/Log.h>
#include <../Common/Wnd32.h>
#include <memory>
#include <assert.h>
#include <thread>
#include <mutex>

void ErrorExit(LPCSTR code) {
	fprintf(stderr, "%s\n", code);
	ExitProcess(0);
}

struct Wnd
{
	HWND hwnd;
	Wnd() : hwnd(nullptr) {}
};

#define WC_WINDOW	"WC_WINDOW"

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT WINAPI SubProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR id, DWORD_PTR data);


HWND mainHwnd, sb;

HWND CreateScrollBar(HWND parent, int width) {
	RECT rc;
	GetClientRect(parent, &rc);

	HINSTANCE inst = (HINSTANCE)GetWindowLong(parent, GWLP_HINSTANCE);
	return CreateWindowEx(0,
		//"SCROLLBAR",
		WC_SCROLLBAR,
		NULL,
		WS_CHILD | WS_VISIBLE | SBS_VERT,
		rc.right - width, rc.top, width, rc.bottom,
		parent, NULL, inst, NULL);
}

int main(int args, char* argv[])
{
	InitCommonControls();
	HINSTANCE hInst = GetModuleHandle(NULL);
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(wc);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hInstance = hInst;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = WC_WINDOW;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WndProc;
	GetClassInfoEx(wc.hInstance, wc.lpszClassName, &wc);
	RegisterClassEx(&wc);

	mainHwnd = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW,
		WS_POPUP | WS_VISIBLE , 600, 400, 700, 380, nullptr, 0, wc.hInstance, NULL);


	sb = CreateScrollBar(mainHwnd, 40);
	SetWindowSubclass(sb, SubProc, 0, 0);
	ShowWindow(mainHwnd, TRUE);
	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.lParam;
}

#define WHEEL_SECTOR 120

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	//GetScrollInfo()
	static HBRUSH br;
	switch (msg)
	{
	case WM_CREATE: {
		br = CreateSolidBrush(RGB(100, 100, 100));
	}break;
	case WM_VSCROLL: {
		SCROLLINFO info{};
		switch (wp)
		{
		case SB_THUMBTRACK: {
			printf("thumb\n");
		}break;
		default:
			break;
		}
	}break;
	case WM_MOUSEWHEEL: {
		int data = (short)HIWORD(wp);
		int hPos = ::MulDiv(data, 4, WHEEL_DELTA);

		SCROLLINFO i{};
		i.cbSize = sizeof(SCROLLINFO);
		i.fMask = SIF_POS;
		GetScrollInfo(sb, SB_VERT, &i);

		printf("%d\n", i.nMin);
		//ScrollWindowEx(0, )
		//SetScrollPos(sb, 0, hPos, TRUE);
	}break;
	case WM_CTLCOLORSCROLLBAR: {
		//printf("WM_CTLCOLOR\n");
		//return (HRESULT)br;
		return true;
	}break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT WINAPI SubProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR id, DWORD_PTR data)
{
	switch (msg)
	{
	case WM_PAINT: {
		LRESULT res = DefSubclassProc(hwnd, msg, wp, lp);
		SCROLLBARINFO info{};
		info.cbSize = sizeof(SCROLLBARINFO);
		GetScrollBarInfo(hwnd, 0, &info);
		RECT rc;
		GetClientRect(hwnd, &rc);
		//SetSysColors()
		//SCROLLINFO i{};
		//i.cbSize = sizeof(SCROLLINFO);
		//i.fMask = SIF_POS;
		//GetScrollInfo(hwnd, SB_VERT, &i);

		int w = GetSystemMetrics(SM_CXVSCROLL);
		
		printf("WM_PAINT res %d\n", res);
	}break;
	case WM_VSCROLL: {
		printf("vscroll\n");
	}break;
	default:
		break;
	}
	return DefSubclassProc(hwnd, msg, wp, lp);
}

