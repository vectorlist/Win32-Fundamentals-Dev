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
#define WC_CHILD	"WC_CHILD"
#define NORMAL_BUTTON		0
#define CUSTOMDRAW_BUTTON	1
#define ITEMDRAW_BUTTON		2
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

HWND mainHwnd;

HWND button[3];
HGDIOBJ pObj[3];

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
		WS_POPUP | WS_VISIBLE, 600, 400, 700, 200, nullptr, 0, wc.hInstance, NULL);

	wc.lpszClassName = WC_CHILD;
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	RegisterClassEx(&wc);

	pObj[0] = CreateSolidBrush(RGB(99, 99, 99));
	pObj[1] = CreatePen(PS_SOLID, 1, RGB(22, 22, 22));
	LOGFONT logFont{};
	logFont.lfHeight = 14;
	logFont.lfWeight = 0;
	logFont.lfCharSet = ANSI_CHARSET;
	LPCSTR font = { "Verdana" };
	memcpy(logFont.lfFaceName, font, sizeof(CHAR) * strlen(font));
	pObj[2] = CreateFontIndirect(&logFont);

	button[0] = CreateWindowEx(NULL, WC_BUTTON, "Normal Draw", 
		WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
		140, 30, 120, 38, mainHwnd, (HMENU)0, hInst, NULL);

	button[1] = CreateWindowEx(NULL, WC_BUTTON, "Custom Draw",
		WS_VISIBLE | WS_CHILD,
		300, 30, 120, 38, mainHwnd, (HMENU)1, hInst, NULL);

	button[2] = CreateWindowEx(NULL, WC_BUTTON, "Item Draw", 
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
		460, 30, 120, 38, mainHwnd, (HMENU)2, hInst, NULL);

	ShowWindow(mainHwnd, TRUE);
	InvalidateRect(mainHwnd, NULL, FALSE);
	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.lParam;
}

LPCSTR GetHwndText(HWND hwnd) {
	static std::basic_string<TCHAR> temp;
	auto len = GetWindowTextLength(hwnd);
	temp.reserve(len + 1);
	GetWindowText(hwnd, (LPSTR)temp.c_str(), len + 1);
	return temp.c_str();
}

void DrawButton(HWND hwnd, HDC dc, LPRECT rc) {
	HGDIOBJ oldObj[3];
	for(int i = 0; i < 3; i ++)
		oldObj[i] = SelectObject(dc, pObj[i]);
	
	Rectangle(dc, rc->left, rc->top, rc->right, rc->bottom);
	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, RGB(220, 220, 220));
	DrawText(dc, GetHwndText(hwnd), -1, rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	for (int i = 0; i < 3; i++)
		SelectObject(dc, oldObj[i]);
}


LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_NOTIFY: {
		LPNMHDR hdr = (LPNMHDR)lp;
		if (hdr->hwndFrom != button[CUSTOMDRAW_BUTTON]) break;
		LPNMCUSTOMDRAW lpCd = (LPNMCUSTOMDRAW)lp;

		switch (lpCd->dwDrawStage)
		{
		case CDDS_PREPAINT: {
			DrawButton(hdr->hwndFrom, lpCd->hdc, &lpCd->rc);
			return CDRF_SKIPDEFAULT | CDRF_SKIPPOSTPAINT;
		}
		}
	}
	case WM_PAINT: {
		HWND foundHwnd = GetWindow(hwnd, GW_CHILD);
		while (foundHwnd != NULL){
			if (foundHwnd == button[NORMAL_BUTTON]) {
				printf("Find out %s\n", GetHwndText(foundHwnd));
				break;
			}
			foundHwnd = GetNextWindow(foundHwnd, GW_HWNDNEXT);
		}
		if (!foundHwnd) break;
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(foundHwnd, &ps);
		DrawButton(foundHwnd, dc, &ps.rcPaint);
		EndPaint(foundHwnd, &ps);
		break;
	}
	case WM_DRAWITEM: {
		DRAWITEMSTRUCT* dis = (LPDRAWITEMSTRUCT)lp;
		if (dis->hwndItem == button[ITEMDRAW_BUTTON]) {
			DrawButton(dis->hwndItem, dis->hDC, &dis->rcItem);
		}
		break;
	}
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
