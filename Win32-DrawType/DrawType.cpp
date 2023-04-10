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

#define WC_WINDOW	"WC_WINDOW"
#define WC_CHILD	"WC_CHILD"
#define NORMAL_BUTTON		0
#define CUSTOMDRAW_BUTTON	1
#define ITEMDRAW_BUTTON		2
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT WINAPI SubClassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR id, DWORD_PTR data);

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
		130, 30, 120, 38, mainHwnd, (HMENU)0, hInst, NULL);

	SetWindowSubclass(button[0], SubClassProc, 0, 0);

	button[1] = CreateWindowEx(NULL, WC_BUTTON, "Custom Draw",
		WS_VISIBLE | WS_CHILD,
		290, 30, 120, 38, mainHwnd, (HMENU)1, hInst, NULL);

	button[2] = CreateWindowEx(NULL, WC_BUTTON, "Owner Draw", 
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
		450, 30, 120, 38, mainHwnd, (HMENU)2, hInst, NULL);

	

	ShowWindow(mainHwnd, TRUE);
	InvalidateRect(mainHwnd, NULL, FALSE);
	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(pObj[0]);
	DeleteObject(pObj[1]);
	return msg.lParam;
}

void DrawButton(HWND hwnd, HDC dc, LPRECT rc) {
	HGDIOBJ oldObj[3];
	for(int i = 0; i < 3; i ++)
		oldObj[i] = SelectObject(dc, pObj[i]);
	
	Rectangle(dc, rc->left, rc->top, rc->right, rc->bottom);
	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, RGB(220, 220, 220));
	DrawText(dc, Wnd32::GetHwndText(hwnd), -1, rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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

LRESULT WINAPI SubClassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR id, DWORD_PTR data)
{
	
	switch (msg)
	{
	case WM_PAINT: {
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		DrawButton(hwnd, dc, &ps.rcPaint);
		EndPaint(hwnd, &ps);
		return TRUE;
		break;
	}
	case WM_CLOSE: {
		RemoveWindowSubclass(hwnd, SubClassProc, 0);
	}
	}
	return DefSubclassProc(hwnd, msg, wp, lp);
}
