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
HMODULE dll = NULL;

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT WINAPI RichProc(HWND, UINT, WPARAM, LPARAM);
WNDPROC preRichProc = NULL;
LRESULT OnPaint(HWND hwnd, HDC dc);
LRESULT OnPaintTake1(HWND hwnd, HDC dc);
void OnPhase(HWND hwnd);
void OnParse2(HWND hwnd);
HFONT font;

#include <vector>

bool IsHighlightText(LPCSTR text) {
	LPCSTR highlightText[] = {
		"int", "float", "double"
	};

	for (auto i : highlightText) {
		if (lstrcmpi(text, i) == 0) {
			return true;
		}
	}
	return false;
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

	CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW,
		WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN, 600, 200, 720, 420, nullptr, 0, wc.hInstance, NULL);
	
	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.lParam;
}
#define CWM_RICHEDITCREATE (WM_APP + 10)

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	static HWND edit = NULL;
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
	}break;
	case WM_COMMAND: {
		DWORD code = HIWORD(wp);
		if (code == EN_UPDATE) {
			HWND ctrl = (HWND)lp;
			//printf("OnUpdate\n");
		}
		if (code == EN_CHANGE) {
			HWND ctrl = (HWND)lp;
			//printf("OnChange\n");
		}
		
	}break;
	case WM_SIZE: {
		MoveWindow(edit, 10, 10, LOWORD(lp) - 20, HIWORD(lp) - 20, TRUE);
		
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
		font = CreateFont(18, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET,
			0, 0, 0, 0, "Consolas");
		SendMessage(hwnd, WM_SETFONT, (UINT_PTR)font, 0);
		SendMessage(hwnd, EM_SETBKGNDCOLOR, 0, RGB(40, 40, 40));
		SendMessage(hwnd, EM_SETEDITSTYLE, SES_EMULATESYSEDIT, SES_EMULATESYSEDIT);
		//SendMessage(hwnd, EM_LIMITTEXT, -1, 0);
		CHARFORMAT cf{};
		cf.cbSize = sizeof(cf);
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = RGB(210, 210, 210);
		cf.dwEffects = 0;
		SendMessage(hwnd, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
		
		//DWORD mask = ::SendMessage(hwnd, EM_GETEVENTMASK, 0, 0);
		//mask |= ENM_CHANGE;
		//SendMessage(hwnd, EM_SETEVENTMASK, 0, (LPARAM)mask);
	}break;
	case WM_PAINT: {
		HideCaret(hwnd);
		//let them built-in paint
		LRESULT res = CallWindowProc(preRichProc, hwnd, msg, wp, lp);
		HDC dc = GetDC(hwnd);
		//res = OnPaint(hwnd, dc);
		//res = OnPaintTake1(hwnd, dc);
		//OnPhase(hwnd);
		OnParse2(hwnd);
		ReleaseDC(hwnd, dc);
		//printf("paint\n");
		ShowCaret(hwnd);
		return res;
	}break;
	case WM_NOTIFY: {
		printf("notify\n");
	}break;
	case WM_KEYDOWN: {
		//CallWindowProc(preRichProc, hwnd, msg, wp, lp);
		//OnPhase(hwnd);
	}break;
	case WM_CHAR: {
		//printf("WM_CHAR\n");
	}break;
	case WM_TIMER: {
		printf("pass word\n");
	}break;
	case WM_DESTROY: {
		DeleteObject(font);
	}break;
	default:
		break;
	}
 	return CallWindowProc(preRichProc, hwnd, msg, wp, lp);
}


LRESULT OnPaint(HWND hwnd, HDC dc) {
	SetBkMode(dc, TRANSPARENT);
	RECT rc;
	SendMessage(hwnd, EM_GETRECT, 0, (LPARAM)&rc);
	POINT pt{ rc.left, rc.top };
	HRGN rgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	//get the first visible line
	int begin = ::SendMessage(hwnd, EM_GETFIRSTVISIBLELINE, 0, 0);
	//get viewport lines
	int end = ::SendMessage(hwnd, EM_GETLINECOUNT, 0, 0);
	LPSTR tokken = nullptr;
	const char* delimiters = " \n\r{}()";
	char* start = nullptr;

	for (int line = begin; line < end; ++line) {
		//get char buffer on the line
		char buf[256];
		::SendMessage(hwnd, EM_GETLINE, line, (LPARAM)buf);
		start = buf;

		if (!start) {
			printf("%d line's : no buffer\n", line);
			continue;
		}
		
		//get line offset of char buffer
		int offset = SendMessage(hwnd, EM_LINEINDEX, line, 0);
		char* context = nullptr;
		tokken = strtok_s(start, delimiters, &context);
		while (tokken)
		{
			CHARRANGE cr{};
			cr.cpMin = offset + (int)(tokken - start);
			cr.cpMax = cr.cpMin + strlen(tokken);
			DWORD dwBuffer = cr.cpMax - cr.cpMin;
			LPSTR pBuffer = (LPSTR)GlobalAlloc(0, (dwBuffer + 1) * sizeof(char));
			TEXTRANGE tr;
			tr.chrg = cr;
			tr.lpstrText = pBuffer;
			SendMessage(hwnd, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
			printf("%s\n", tr.lpstrText);
			//CHAR* buffer   =  buf + cr.cpMin;
			LPCSTR buffer = tr.lpstrText;
			if (IsHighlightText(buffer)) {
				SendMessage(hwnd, EM_POSFROMCHAR, (WPARAM)&rc, (LPARAM)cr.cpMin);
				SelectObject(dc, font);
				SetTextColor(dc, RGB(200, 200, 0));
				DrawText(dc, buffer, dwBuffer, &rc, 0);
			}
			GlobalFree(pBuffer);
			//printf("%s\n", tokken);
			tokken = strtok_s(NULL, delimiters, &context);
		}
	}

	SelectObject(dc, rgn);
	DeleteObject(rgn);
	ShowCaret(hwnd);
	return 0;
}

LRESULT OnPaintTake1(HWND hwnd, HDC dc) 
{
	RECT rc;
	::SendMessage(hwnd, EM_GETRECT, 0, (LPARAM)&rc);
	POINTL ptl = { rc.left, rc.top };
	int pos = ::SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM)&rc);
	
	int line = ::SendMessage(hwnd, EM_LINEFROMCHAR, pos, 0);
	

	TEXTRANGE tr;
	tr.chrg.cpMin = SendMessage(hwnd, EM_LINEINDEX, line, 0);
	tr.chrg.cpMax = SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM)&rc.right);

	int size = tr.chrg.cpMax - tr.chrg.cpMin;
	int first = tr.chrg.cpMin;
	//printf("charfrompos %d linefromchar %d size %d\n", pos, line, size);
	if (size <= 0) return 0;
	std::string str;
	str.reserve(size);
	tr.lpstrText = (LPSTR)str.data();
	::SendMessage(hwnd, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
	//printf("%s size %d\n", tr.lpstrText, size);
	LPSTR lpszStart = strstr(tr.lpstrText, " ");
	if (lpszStart) {
		printf("sucss\n");
	}
	//while (lpszStart)
	//{
	//	lpszStart++;
	//}
	//_RICHEDIT_VER
	return 1;
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

void ParseLine(HWND hwnd, LPCSTR pLines, int nCharPos, int curLine);

void OnPhase(HWND hwnd)
{
	int begin = ::SendMessage(hwnd, EM_GETFIRSTVISIBLELINE, 0, 0);
	int end   = ::SendMessage(hwnd, EM_GETLINECOUNT, 0, 0);

	int lenth = 0;

	GETTEXTLENGTHEX gtl{};
	gtl.flags = GTL_DEFAULT;
	gtl.codepage = CP_ACP;
	DWORD len = SendMessage(hwnd, EM_GETTEXTLENGTHEX, (WPARAM)&gtl, 0);
	//printf("%d\n", len);
	GETTEXTEX gt{};
	gt.cb = sizeof(GETTEXTEX);
	gt.flags = GT_RAWTEXT;
	gt.codepage = CP_ACP;
	
	RECT rc;
	SendMessage(hwnd, EM_GETRECT, 0, (LONG)&rc);
	DWORD curCharIndex = SendMessage(hwnd, EM_CHARFROMPOS, 0, (LONG)&rc.right);
	//printf("%d\n", curCharIndex);
	//if (curCharIndex <= 0) return;
	//LPSTR pBuffer = (LPSTR)GlobalAlloc(0, curCharIndex +1 * sizeof(char));
	//HeapAlloc()
	LPSTR pBuffer = (LPSTR)malloc(curCharIndex + 1 * sizeof(char));

	TEXTRANGE tr;
	tr.chrg.cpMin = 0;
	tr.chrg.cpMax = curCharIndex;
	tr.lpstrText = pBuffer;
	SendMessage(hwnd, EM_GETTEXTRANGE, 0, (LONG)&tr);

	int offset = 0;
	for (int line = begin; line < end; ++line) {
		/*offset += SendMessage(hwnd, EM_LINEINDEX, line, 0);*/
		//printf("%d\n", offset);
		LPCSTR szStart = tr.lpstrText + offset;
		printf("%s\n", szStart);
		offset += SendMessage(hwnd, EM_LINEINDEX, line, 0);
		
	}

	free(pBuffer);
}

void OnParse2(HWND hwnd) {
	int begin = GetFirstVisibleLine(hwnd);
	int end = GetLastVisibleLine(hwnd);
	LPSTR tok;
	int lineOffset = 0;
	int lastChar = 0;
	for (int line = begin; line <= end; line++) {
		//printf("%d\n", line);
		lineOffset = ::SendMessage(hwnd, EM_LINEINDEX, line, 0);
		char buf[256] = {0};
		int length = ::SendMessage(hwnd, EM_GETLINE, line, (LPARAM)buf);
		printf("%s\n", buf);
		tok = strtok(buf, " ");
		while (tok != NULL)
		{
			/*printf("%s\n", tok);*/
			if (lstrcmpA(tok, "int") == 0) {
				CHARRANGE cr;
				cr.cpMin = lineOffset + length - strlen(tok);
				cr.cpMax = cr.cpMin + strlen(tok);
				//printf("line : %d %s\n", line, tok);
				SendMessage(hwnd, EM_EXSETSEL, 0, (LPARAM)&cr);
				printf("line : %d %d\n", cr.cpMin, cr.cpMax);
			}
			tok = strtok(NULL, " ");
		}

	}
	CHARRANGE cr{ 0 };
	//SendMessage(hwnd, EM_EXGETSEL, 0, (LPARAM)&cr);
	//SendMessage(hwnd, EM_EXSETSEL, cr.cpMax, -1);
	//SendMessage(hwnd, EM_HIDESELECTION, 1, 0);
	//m_reText.SetSel(0, -1);      //Select everything in the control.
	//m_reText.GetSel(start, end); //Now, get that selection.
	//m_reText.SetSel(end, -1);  //Set the selection to everything after the last character.
	
}

void ParseLine(HWND hwnd, LPCSTR pLines, int nCharPos, int curLine) {
	CHAR* pCurChar = (CHAR*)pLines;
	long lCharStart = SendMessage(hwnd, EM_LINEINDEX, curLine, 0);

}
