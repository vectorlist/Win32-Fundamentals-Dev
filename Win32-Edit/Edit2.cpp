#pragma once
#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <vector>
#include <strsafe.h>
#include <Richedit.h>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <../Common/Log.h>
#include <../Common/Wnd32.h>

#define WC_WINDOW "WC_WINDOW"
#define CWM_RTF_UPDATE	(WM_APP + 401)
#define RTF_LINENUMBER_SPACING	50

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

HWND mainHwnd;

HWND edit;
HWND button;
HGDIOBJ pObj[3];
WNDPROC preEditProc;
void SetTxtColor(HWND hwnd, COLORREF col);
void SetFont(HWND hWindow, const char * Font);
LRESULT OnLineNumberUpdate(WPARAM wp, LPARAM lp);
void DrawLineNumber(HDC dc);

LRESULT editProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
CHARFORMAT2 GetCharFormat(HWND hwnd) {
	CHARFORMAT2 cf{};
	cf.cbSize = sizeof(cf);
	SendMessage(hwnd, EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
	return cf;
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
		WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN , 600, 400, 760, 480, nullptr, 0, wc.hInstance, NULL);

	wc.lpszClassName = WC_WINDOW;
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	RegisterClassEx(&wc);

	pObj[0] = CreateSolidBrush(RGB(33, 33, 33));
	pObj[1] = CreatePen(PS_SOLID, 1, RGB(22, 22, 22));
	lf lf{};
	lf.lfHeight = 16;
	lf.lfWeight = 0;
	lf.lfCharSet = ANSI_CHARSET;
	LPCSTR font = { "Consolas" };
	memcpy(lf.lfFaceName, font, sizeof(CHAR) * strlen(font));
	pObj[2] = CreateFontIndirect(&lf);



	auto dll = LoadLibrary("riched20.dll");

	edit = CreateWindowEx(NULL, RICHEDIT_CLASS, "",
		ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VISIBLE | WS_CHILD,
		10, 10, 550, 360, mainHwnd, (HMENU)0, hInst, NULL);
	preEditProc = (WNDPROC)SetWindowLongPtr(edit, GWLP_WNDPROC, (LONG_PTR)editProc);

	RECT rc;
	GetClientRect(edit, &rc);
	rc.left += RTF_LINENUMBER_SPACING;
	SendMessage(edit, EM_SETRECT, 0, (LONG)&rc);
	SendMessage(edit, EM_SETBKGNDCOLOR, 0, RGB(32, 32, 32));
	SetTxtColor(edit, RGB(220, 220, 220));
	SetFont(edit, "Consolas");
	DWORD evt = ENM_SELCHANGE | ENM_UPDATE | ENM_CHANGE;
	SendMessage(edit, EM_SETEVENTMASK, 0, (LPARAM)evt);
	//ENM_SELCHANGE

	
	
	button = CreateWindowEx(NULL, WC_BUTTON, "Get Text",
		WS_VISIBLE | WS_CHILD,
		600, 200, 110, 30, mainHwnd, (HMENU)0, hInst, NULL);

	//SendMessage(edit, WM_SETFONT, (WPARAM)pObj[2], 0);

	ShowWindow(mainHwnd, TRUE);
	UpdateWindow(mainHwnd);
	//OnLineNumberUpdate(0, 0);
	auto dc = GetDC(edit);
	DrawLineNumber(dc);
	ReleaseDC(edit, dc);
	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (dll) FreeLibrary(dll);
	return msg.lParam;
}

void SetTxtColor(HWND hwnd, COLORREF col) {
	CHARFORMAT cf{};
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = col;
	cf.dwEffects = 0; // add this line
	SendMessage(hwnd, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
	LRESULT res;
}

void SetFont(HWND hWindow, const char* Font) {
	CHARFORMAT2 cf;
	memset(&cf, 0, sizeof cf);
	cf.cbSize = sizeof cf;
	cf.dwMask = CFM_FACE;
	wsprintf(cf.szFaceName, Font);
	SendMessage(hWindow, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
}

int GetLineIndex(HWND hwnd, int line) {
	return SendMessage(hwnd, EM_LINEINDEX, line, 0);
}

POINT GetCharPos(HWND hwnd, int index) {
	POINT pos{};
	SendMessage(hwnd, EM_POSFROMCHAR, (WPARAM)&pos, index);
	return pos;
}

LRESULT OnLineNumberUpdate(WPARAM wp, LPARAM lp) {
	RECT rc;
	GetClientRect(edit, &rc);
	rc.right = RTF_LINENUMBER_SPACING;
	HDC dc = GetDC(edit);
	//Wnd32::DrawFillRect(dc, rc, RGB(40, 100, 170));
	auto br = CreateSolidBrush(RGB(55, 55, 58));
	//ReleaseDC(edit, dc);
	FillRect(dc, &rc, br);
	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, RGB(200, 50, 0));
	
	int height = rc.bottom - rc.top;
	int fvl = (int)SendMessage(edit, EM_GETFIRSTVISIBLELINE, 0, 0);
	int lineCount = SendMessage(edit, EM_GETLINECOUNT, 0, 0);
	
	POINT pt1 = GetCharPos(edit, GetLineIndex(edit, -1));
	POINT pt2 = GetCharPos(edit, GetLineIndex(edit, fvl));

	//DWORD pp = SendMessage(edit, EM_POSFROMCHAR, 0, (LPARAM)-1);
	//POINT ppp{ LOWORD(pp), HIWORD(pp) };

	RECT lineRc = rc;
	lineRc.left += 10;
	std::string msg;
	CHAR buffer[128];
	for (int i = fvl; i < lineCount;) {
		lineRc.top = pt2.y + 4;
		lineRc.bottom = pt2.y + 22;
		if (lineRc.top > lineRc.bottom) break;
		sprintf(buffer, "%ld", i + 1);
		DrawText(dc, buffer, -1, &lineRc, DT_LEFT | DT_VCENTER);
		pt2 = GetCharPos(edit,GetLineIndex(edit,++i));
		if (pt2.y >= height) break;
	}

	//FillRect(dc, &rc, br);
	DeleteObject(br);
	return TRUE;
}

void DrawLineNumber(HDC dc) {
	RECT rc;
	GetClientRect(edit, &rc);
	rc.right = RTF_LINENUMBER_SPACING;

	SIZE size{ rc.right - rc.left, rc.bottom - rc.top };
	HDC mem_dc = CreateCompatibleDC(dc);
	HBITMAP hBmp = CreateCompatibleBitmap(dc, size.cx, size.cy);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(mem_dc, hBmp);
	HBRUSH br = CreateSolidBrush(RGB(100, 100, 100));
	//Wnd32::DrawFillRect(mem_dc, rc, RGB(100, 100, 100));
	FillRect(mem_dc, &rc, br);
	BitBlt(dc, rc.left, rc.top, size.cx, size.cy, mem_dc, 0, 0, SRCCOPY);

	SelectObject(mem_dc, hOldBmp);
	DeleteObject(hBmp);
	DeleteDC(mem_dc);
	DeleteObject(br);
	
	
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_NOTIFY: {
		NMHDR* hdr = (LPNMHDR)lp;
		MSGFILTER* filter = (MSGFILTER*)hdr;
		if (hdr->code == EN_MSGFILTER) {
			MSGFILTER* filter = (MSGFILTER*)hdr;
			//printf("WM_ + %d\n", filter->msg);
			LOG::LogWndMessage(filter->msg, 0);
		}
		if (hdr->code == EN_SELCHANGE) {
			SELCHANGE* pSc = (SELCHANGE*)hdr;
			printf("EN_SELCHANGE \n");
			//return OnLineNumberUpdate(0, 0);
			HDC dc = GetDC(edit);
			DrawLineNumber(dc);
			ReleaseDC(edit,dc);
			break;
		}
		if (hdr->code == EN_CHANGE) {
			printf("EN_CHANGE\n");
			break;
		}
		

		//printf("WM_NOTIFY %s %d\n", Wnd32::GetHwndText(hdr->hwndFrom), hdr->code);
		break;
	}
	case WM_COMMAND: {
		if ((HWND)lp == edit) {
			switch (HIWORD(wp))
			{
			case EN_UPDATE: {
				printf("EN_UPDATE\n");
				//OnLineNumberUpdate(0, 0);
				RECT rc;
				SendMessage(edit, EM_GETRECT, 0, (LPARAM)&rc);
				//BOOL b = ValidateRect(edit, &rc);
				//if (b) return b;
				HDC dc = GetDC(edit);
				DrawLineNumber(dc);
				ReleaseDC(edit, dc);
				//return TRUE;
				//return 0;
				break;
			}
			case EN_CHANGE: {
				printf("EN_CHANGE\n");
				HDC dc = GetDC(edit);
				DrawLineNumber(dc);
				ReleaseDC(edit, dc);
				break;
			}
			}
			//Sleep(100);
		}
		break;
	}
	case WM_SIZE: {
		printf("WM_SIZE %s\n", Wnd32::GetHwndText(hwnd));
		if (edit) {
			RECT rc;
			GetClientRect(edit, &rc);
			rc.left += RTF_LINENUMBER_SPACING;
			SendMessage(edit, EM_SETRECT, 0, (LPARAM)&rc);
			printf("EM_SETRECT\n");
		}
		break;
	}
	case WM_PAINT: {
		printf("WM_PAINT %s\n", Wnd32::GetHwndText(hwnd));
		break;
	}
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT editProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_PAINT: {
		//HideCaret(hwnd);
		//LRESULT ls = CallWindowProc(preEditProc, hwnd, msg, wp, lp);
		//RECT rc;
		//SendMessage(hwnd, EM_GETRECT, 0, (LONG)&rc);
		//rc.left -= 1;
		//rc.right += 1;
		//HDC dc = GetDC(hwnd);
		//Wnd32::DrawFillRect(dc, rc, RGB(32, 32, 32));
		//SelectObject(dc, pObj[2]);
		//auto txt = Wnd32::GetHwndText(hwnd);
		//SetBkMode(dc, TRANSPARENT);
		//SetTxtColor(hwnd, RGB(200, 0, 0));
		//DrawText(dc, txt, -1, &rc, 0);
		////TextOut(dc, 0, 0, txt, 0);
		//ReleaseDC(hwnd, dc);
		//printf("WM_PAINT\n");
		//return OnLineNumberUpdate(wp, lp);
		//return ls;
		break;
	}
	case EM_SETCHARFORMAT: {
		printf("EM_CHARFORMAT %d\n", wp);
		break;
	}
	case WM_KEYDOWN: {
		printf("WM_KEYDOWN\n");
		break;
	}
	case CWM_RTF_UPDATE: {
		OnLineNumberUpdate(wp, lp);
		InvalidateRect(hwnd, NULL, FALSE);
		printf("CWM_RTF_UPDATE\n");
		break;
	}
	case WM_ERASEBKGND: {
		printf("WM_ERASEBKGND\n");
		//return 0;
		break;
	}
	default:
		break;
	}
	return CallWindowProc(preEditProc, hwnd, msg, wp, lp);
}
