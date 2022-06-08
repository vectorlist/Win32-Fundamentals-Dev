#pragma once
#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <vector>
#include <strsafe.h>
#include <Richedit.h>
#include <sstream>

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
HGDIOBJ pObj[2];
UINT gLineHeight = 0;
WNDPROC preEditProc;
void SetTxtColor(HWND hwnd, COLORREF col);
void SetFont(HWND hWindow, const char * Font);


LRESULT editProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

int GetLineIndex(HWND hwnd, int line) {
	return SendMessage(hwnd, EM_LINEINDEX, line, 0);
}

POINT GetCharPos(HWND hwnd, int index) {
	POINT pos{};
	SendMessage(hwnd, EM_POSFROMCHAR, (WPARAM)&pos, index);
	return pos;
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
		WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN, 600, 300, 780, 640, nullptr, 0, wc.hInstance, NULL);

	wc.lpszClassName = WC_WINDOW;
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	RegisterClassEx(&wc);

	pObj[0] = CreateSolidBrush(RGB(33, 33, 33));
	LOGFONT lf{};
	lf.lfHeight = 14;
	lf.lfWeight = FW_THIN;
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfPitchAndFamily = 0;
	lf.lfQuality = DEFAULT_QUALITY;
	
	LPCSTR font = { "Consolas" };
	memcpy(lf.lfFaceName, font, sizeof(CHAR) * strlen(font));
	pObj[1] = CreateFontIndirect(&lf);



	auto dll = LoadLibrary("riched20.dll");
	RECT rc; GetClientRect(mainHwnd, &rc);
	rc.top += 40;
	InflateRect(&rc, -14, -10);
	edit = CreateWindowEx(NULL, RICHEDIT_CLASS, "",
		ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VISIBLE | WS_CHILD
		| WS_CLIPSIBLINGS,
		rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, mainHwnd, (HMENU)0, hInst, NULL);
	//Subclassing
	preEditProc = (WNDPROC)SetWindowLongPtr(edit, GWLP_WNDPROC, (LONG_PTR)editProc);

	
	GetClientRect(edit, &rc);
	rc.left += RTF_LINENUMBER_SPACING;
	//set magin
	SendMessage(edit, EM_SETRECT, 0, (LONG)&rc);
	//txt bgrnd
	SendMessage(edit, EM_SETBKGNDCOLOR, 0, RGB(32, 32, 32));
	//txt Color
	CHARFORMAT cf{};
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = RGB(210, 210, 210);
	cf.dwEffects = 0;
	SendMessage(edit, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);

	cf = {};
	cf.cbSize = sizeof(cf);

	cf.dwMask = CFM_FACE | CFM_CHARSET | CFM_SIZE;
	wsprintf(cf.szFaceName, lf.lfFaceName);
	//CHARFORMAT requires the hegiht to be in twip n /20
	//1 inch = 72 pixel, 1 pixel = 20 twip, 1 inch = 1440 twip
	//twips  = pixels * 72 * 20 / GetDivecCap(LOGPIXELY)
	int nTwips = lf.lfHeight * 72 * 20 / GetDeviceCaps(GetDC(edit), LOGPIXELSY);

	cf.yHeight = nTwips;
	cf.bCharSet = lf.lfCharSet;
	cf.bPitchAndFamily = lf.lfPitchAndFamily;

	SendMessage(edit, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);

	//Disable AutoFont
	DWORD option = SendMessage(edit, EM_GETLANGOPTIONS, 0, 0);
	option &= ~IMF_AUTOFONT;
	SendMessage(edit, EM_SETLANGOPTIONS, 0, option);

	//get line height manually
	SetWindowText(edit, "1\n2\n");
	int charPos[2];
	SendMessage(edit, EM_POSFROMCHAR, (WPARAM)&charPos, 2);
	gLineHeight = charPos[1];
	SetWindowText(edit, "");
	//TODO : disable allowbeep sound
	IUnknown* unk;
	

	ShowWindow(mainHwnd, TRUE);
	InvalidateRect(mainHwnd, NULL, FALSE);

	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (dll) FreeLibrary(dll);
	return msg.lParam;
}

LRESULT Draw(HDC dc) {
	RECT rc;
	GetClientRect(edit, &rc);
	rc.right = RTF_LINENUMBER_SPACING - 10;
	//HDC dc = GetDC(edit);
	Wnd32::DrawFillRect(dc, rc, RGB(32, 32, 32));

	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, RGB(37, 128, 145));

	int fvl = SendMessage(edit, EM_GETFIRSTVISIBLELINE, 0, 0);
	int nLine = SendMessage(edit, EM_GETLINECOUNT, 0, 0);
	int editHeight = rc.bottom - rc.top;
	printf("total line %d first visible line : %d\n", nLine, fvl);

	CHAR buffer[128];
	int offset = 0;
	int lineHeght = 0;
	//Get Font Physically Pixel Points
	CHARFORMAT lcf{};
	lcf.cbSize = sizeof(lcf);
	SendMessage(edit, EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&lcf);
	PARAFORMAT2 lpf{};
	lpf.cbSize = sizeof(lcf);
	lpf.dwMask = PFM_LINESPACING;
	SendMessage(edit, EM_GETPARAFORMAT, 0, (LONG_PTR)&lpf);

	//lineHeght = (lcf.yHeight * GetDeviceCaps(dc, LOGPIXELSY) / 1440) + 3;
	lineHeght = gLineHeight;

	for (int i = fvl; i < nLine;) {
		RECT lineRc = rc;
		lineRc.top = offset;
		lineRc.bottom = offset + lineHeght;
		//Rectangle(dc, lineRc.left, lineRc.top, lineRc.right, lineRc.bottom);
		//_itoa(i + 1, buffer, 10);
		sprintf(buffer, "%ld", i + 1);
		DrawText(dc, buffer, -1, &lineRc, DT_RIGHT | DT_VCENTER);
		printf("line : %d offset : %d\n", i, lineRc.bottom);
		i++;
		offset += lineHeght;
		if (editHeight < lineRc.bottom) break;
	}

	return TRUE;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_PAINT: {
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		if (edit) {
			printf("edits\n");
			SetBkMode(dc, TRANSPARENT);
			SetTextColor(dc, RGB(220, 220, 220));
			auto oldFont = SelectObject(dc, pObj[1]);
			RECT rc; SendMessage(edit, EM_GETRECT, 0, (LPARAM)&rc);

			rc.bottom = 50;
			OffsetRect(&rc, 0, 20);
			DrawText(dc, "RichEditor 20A", -1,&rc, DT_LEFT | DT_TOP);
			SelectObject(dc, oldFont);
		}
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_NCHITTEST: {
		LRESULT hit = DefWindowProc(hwnd, msg, wp, lp);
		if (hit == HTCLIENT) {
			hit = HTCAPTION;
			return hit;
		}
		break;
	}
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT editProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_PAINT:{
		//let them default draw
		LRESULT res = CallWindowProc(preEditProc, hwnd, msg, wp, lp);
		if (!res) {
			//override own paint
			HDC dc = GetDC(edit);
			Draw(dc);
			ReleaseDC(edit, dc);
		}
		break;
	}
	default:
		break;
	}
	return CallWindowProc(preEditProc, hwnd, msg, wp, lp);
}
