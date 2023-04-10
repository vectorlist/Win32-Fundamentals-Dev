#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <CommCtrl.h>
#include <Richedit.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <stdio.h>
#include <commdlg.h>

#define WC_WINDOW "WC_WINDOW"
#define CWM_RICHEDITCREATE (WM_APP + 10)
HMODULE dll = NULL;

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT WINAPI RichProc(HWND, UINT, WPARAM, LPARAM);
WNDPROC preRichProc = NULL;
void OnParse(HWND hwnd, HDC dc);
void OnParse2(HWND hwnd);
HFONT font;

static const char* keyword[] = { "int", "float", "double" };

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

	CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW,
		WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN, 600, 200, 720, 420, nullptr, 0, wc.hInstance, NULL);

	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.lParam;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	static HWND edit = NULL;
	static RECT rc,titleRc;
	switch (msg)
	{
	case WM_CREATE: {
		dll = LoadLibrary("riched20.dll");
		//dll = LoadLibrary("Msftedit.dll");

		edit = CreateWindowEx(NULL, RICHEDIT_CLASS, 0,
			WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_NOHIDESEL,
			0, 0, 0, 0, hwnd, 0, ((CREATESTRUCT*)lp)->hInstance, 0);
		preRichProc = (WNDPROC)SetWindowLongPtr(edit, GWLP_WNDPROC, (LONG)RichProc);
		SendMessage(edit, CWM_RICHEDITCREATE, 0, 0);
		GetClientRect(hwnd, &rc);
		titleRc = rc; 
		titleRc.bottom = 60;
		InflateRect(&rc, -10, 0);
		rc.bottom -= 10; rc.top += 60;
		SetWindowPos(edit, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, NULL);
	}break;
	case WM_COMMAND: {
		DWORD code = HIWORD(wp);
		if (code == EN_UPDATE) {
			HWND ctrl = (HWND)lp;
			//printf("OnUpdate\n");
		}
		//add a ENM_CHANGE on EM_SETMASK
		if (code == EN_CHANGE) {
			HWND ctrl = (HWND)lp;
			//printf("OnChange\n");
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		SetBkMode(dc,TRANSPARENT);
		SetTextColor(dc, RGB(200, 200, 200));
		DrawText(dc, "Highlight keyword : int float double", 
			-1, &titleRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		EndPaint(hwnd, &ps);
	}
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT WINAPI RichProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case CWM_RICHEDITCREATE: {
		font = CreateFont(18, 0, 0, 0, FW_REGULAR, 0, 0, 0, ANSI_CHARSET,
			0, 0, CLEARTYPE_QUALITY, 0, "Consolas");
		SendMessage(hwnd, WM_SETFONT, (UINT_PTR)font, 0);
		SendMessage(hwnd, EM_SETBKGNDCOLOR, 0, RGB(40, 40, 40));
		SendMessage(hwnd, EM_SETEDITSTYLE, SES_EMULATESYSEDIT, SES_EMULATESYSEDIT);
		SendMessage(hwnd, EM_LIMITTEXT, -1, 0);
		CHARFORMAT cf{};
		cf.cbSize = sizeof(cf);
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = RGB(210, 210, 210);
		cf.dwEffects = 0;
		SendMessage(hwnd, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);

		LOGFONT lf;
		//lFont.
		GetObject(font, sizeof(LOGFONT), &lf);

		int h = 0;
		//DWORD mask = ::SendMessage(hwnd, EM_GETEVENTMASK, 0, 0);
		//mask |= ENM_SELCHANGE;
		//SendMessage(hwnd, EM_SETEVENTMASK, 0, (LPARAM)mask);
	}break;
	case WM_PAINT: {
		//let them built-in paint
		LRESULT res = CallWindowProc(preRichProc, hwnd, msg, wp, lp);
		HideCaret(hwnd);	
		HDC dc = GetDC(hwnd);
		//OnParse(hwnd, dc);
		OnParse2(hwnd);
		ReleaseDC(hwnd, dc);
		ShowCaret(hwnd);
		//return 0;
	}break;
	case WM_DESTROY: {
		DeleteObject(font);
	}break;
	default:
		break;
	}
	return CallWindowProc(preRichProc, hwnd, msg, wp, lp);
}

int GetFirstVisibleLine(HWND hwnd) {
	return (int)::SendMessage(hwnd, EM_GETFIRSTVISIBLELINE, 0, 0);
}

int GetLastVisibleLine(HWND hwnd) {
	RECT rc{ 0 };
	::SendMessage(hwnd, EM_GETRECT, 0, (LPARAM)&rc);
	rc.left++;
	rc.bottom -= 2;
	POINT pt{ rc.left, rc.bottom };
	int nCharIndex = ::SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM)&pt);
	return (int)::SendMessage(hwnd, EM_LINEFROMCHAR, nCharIndex, 0);
}

void OnParse(HWND hwnd, HDC dc)
{
	RECT rc;
	SendMessage(hwnd, EM_GETRECT, 0, (LPARAM)&rc);
	int nCharIndex = SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM)&rc);
	int lastLine = SendMessage(hwnd, EM_LINEFROMCHAR, nCharIndex, 0);
	//printf("nChar : %d last line : %d\n", nCharIndex, lastLine);
	int cMin = SendMessage(hwnd, EM_LINEINDEX, lastLine, 0);
	int cMax = SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM)&rc.right);
	int dwSize = cMax - cMin;
	//printf("min : %d max : %d size : %d\n", cMin, cMax, dwSize);
	if (dwSize <= 0) return;
	
	LPSTR lpBuffer = (LPSTR)malloc(sizeof(char) * (dwSize + 1));
	TEXTRANGE tr{};
	tr.chrg.cpMin = cMin;
	tr.chrg.cpMax = cMax;
	tr.lpstrText = lpBuffer;
	int nAlloc = SendMessage(hwnd, EM_GETTEXTRANGE, 0, (LPARAM)&tr);

	LPSTR start = tr.lpstrText;

	LPSTR tok;
	tok = strtok(start, " \n\r");

	HRGN rgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	HRGN oldRgn = (HRGN)SelectObject(dc, rgn);
	HFONT oldFont = (HFONT)SelectObject(dc, font);
	SetBkMode(dc,TRANSPARENT);
	//SetTextColor(dc, RGB(100, 180, 210));
	
	while (tok)
	{
		int len = strlen(tok);
		for (auto word : keyword) {
			if (lstrcmp(tok, word) == 0) {
				CHARRANGE cr;
				cr.cpMin = cMin + (tok - start);
				cr.cpMax = cr.cpMin + len;
				SendMessage(hwnd, EM_POSFROMCHAR, (WPARAM)&rc, (LPARAM)cr.cpMin);
				//printf("%d %d\n", rc.left, rc.top);
				SetTextColor(dc, RGB(80, 180, 220));
				//DrawText(dc, tok, len, &rc, NULL);
				TextOut(dc, rc.left, rc.top, tok, len);
			}
		}
		tok = strtok(NULL, " \n\r");
	}
	
	SelectObject(dc, oldRgn);
	SelectObject(dc, oldFont);
	DeleteObject(rgn);
	free(lpBuffer);
	//SetSel(0, -1);      //Select everything in the control.
	//GetSel(start, end); //Now, get that selection.
	//SetSel(end, -1);  //Set the selection to everything after the last character.
}

void OnParse2(HWND hwnd) {
	RECT rc;
	SendMessage(hwnd, EM_GETRECT, 0, (LPARAM)&rc);
	int nCharIndex = SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM)&rc);
	int lastLine = SendMessage(hwnd, EM_LINEFROMCHAR, nCharIndex, 0);
	//printf("nChar : %d last line : %d\n", nCharIndex, lastLine);
	int cMin = SendMessage(hwnd, EM_LINEINDEX, lastLine, 0);
	int cMax = SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM)&rc.right);
	int dwSize = cMax - cMin;
	//printf("min : %d max : %d size : %d\n", cMin, cMax, dwSize);
	if (dwSize <= 0) return;

	LPSTR lpBuffer = (LPSTR)malloc(sizeof(char) * (dwSize + 1));
	TEXTRANGE tr{};
	tr.chrg.cpMin = cMin;
	tr.chrg.cpMax = cMax;
	tr.lpstrText = lpBuffer;
	int nAlloc = SendMessage(hwnd, EM_GETTEXTRANGE, 0, (LPARAM)&tr);

	LPSTR start = tr.lpstrText;

	LPSTR tok;
	tok = strtok(start, " \n\r");
	while (tok)
	{
		int len = strlen(tok);
		for (auto word : keyword) {
			if (lstrcmp(tok, word) == 0) {
				CHARRANGE cr;
				cr.cpMin = cMin + (tok - start);
				cr.cpMax = cr.cpMin + len;
				SendMessage(hwnd, EM_EXSETSEL, 0, (LONG)&rc);
			}
		}
		tok = strtok(NULL, " \n\r");
	}
}
