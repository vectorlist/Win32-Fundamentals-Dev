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

#define WC_WINDOW	"WC_WINDOW"

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

HWND mainHwnd, sb;

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
		WS_POPUP | WS_VISIBLE | WS_VSCROLL, 600, 400, 700, 380, nullptr, 0, wc.hInstance, NULL);

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
	static HBRUSH br;
	static int client_y;
	int totalWord = 50;
	static int yPos;
	static int rows;
	static int fontHeight;
	switch (msg)
	{
	case WM_CREATE: {
		HDC dc = GetWindowDC(hwnd);
		TEXTMETRIC tm;
		GetTextMetrics(dc, &tm);
		fontHeight = tm.tmHeight;
		ReleaseDC(hwnd, dc);
		br = CreateSolidBrush(RGB(90, 130, 200));
	}break;
	case WM_PAINT: {
		//ShowScrollBar(hwnd, SB_VERT, FALSE);
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		int count;
		int lineCount = 0;
		int startLine = GetScrollPos(hwnd, SB_VERT);
		int endLine = startLine + rows;
		int strLength = 0;
		CHAR buf[10];
		SetTextColor(dc, RGB(190, 190, 190));
		SetBkMode(dc, TRANSPARENT);
		for (count = startLine; count < endLine; count++) {
			strLength = sprintf(buf, "%d", count);
			TextOut(dc, 12, lineCount, buf, strLength);
			lineCount = lineCount + fontHeight;
		}
		SCROLLBARINFO info{};
		info.cbSize = sizeof(SCROLLBARINFO);
		GetScrollBarInfo(hwnd, OBJID_VSCROLL, &info);

		//ShowScrollBar(hwnd, SB_VERT, FALSE);
		RECT rc = info.rcScrollBar;
		MapWindowPoints(HWND_DESKTOP, hwnd, (POINT*)&rc, 2);
		rc.left -= 10;
		FillRect(dc, &rc, br);
		
		EndPaint(hwnd, &ps);
	}break;
	case WM_SIZE: {
		client_y = HIWORD(lp);
		rows = client_y / fontHeight;
		int tLine = totalWord * fontHeight;
		SCROLLINFO info{};
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_ALL;
		info.nMin = 0;
		info.nMax = totalWord - rows;
		info.nPage = 10;

		SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
		InvalidateRect(hwnd, NULL, TRUE);
	}break;
	case WM_VSCROLL: {
		SCROLLINFO info{};
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_VERT, &info);

		WORD code = LOWORD(wp);
		switch (code)
		{
		case SB_TOP: info.nPos = info.nMin; break;
		case SB_BOTTOM: info.nPos = info.nMax; break;
		case SB_THUMBTRACK: info.nPos = info.nTrackPos; break;
		case SB_PAGEUP: info.nPos -= 2; break;
		case SB_PAGEDOWN: info.nPos += 2; break;
		default: break;
		}

		info.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
		InvalidateRect(hwnd, NULL, TRUE);
	}break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}


