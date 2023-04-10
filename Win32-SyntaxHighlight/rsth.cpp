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
#define WC_WINDOW "WC_WINDOW"
HMODULE dll = NULL;

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT WINAPI RichProc(HWND, UINT, WPARAM, LPARAM);
WNDPROC preRichProc = NULL;
LRESULT OnPaint(HWND hwnd, HDC dc);
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
		dll = LoadLibrary("riched20");
		edit = CreateWindowEx(NULL, RICHEDIT_CLASS, 0,
			WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_NOHIDESEL,
			0, 0, 0, 0, hwnd, 0, ((CREATESTRUCT*)lp)->hInstance, 0);
		preRichProc = (WNDPROC)SetWindowLongPtr(edit, GWLP_WNDPROC, (LONG)RichProc);
		SendMessage(edit, CWM_RICHEDITCREATE, 0, 0);
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
		SendMessage(hwnd, EM_LIMITTEXT, -1, 0);
		CHARFORMAT cf{};
		cf.cbSize = sizeof(cf);
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = RGB(210, 210, 210);
		cf.dwEffects = 0;
		SendMessage(hwnd, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
	}break;
	case WM_PAINT: {
		HideCaret(hwnd);
		LRESULT res = CallWindowProc(preRichProc, hwnd, msg, wp, lp);
		HDC dc = GetDC(hwnd);
		res = OnPaint(hwnd, dc);
		ReleaseDC(hwnd, dc);
		return res;
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
	LONG_PTR cp = (LONG_PTR)SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM)&pt);
	LONG_PTR line = SendMessage(hwnd, EM_LINEFROMCHAR, cp, 0);
	//printf("line : %d\n", cp);
	TEXTRANGE tr{};
	tr.chrg.cpMin = SendMessage(hwnd, EM_LINEINDEX, line, 0);
	tr.chrg.cpMax = SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM)&rc.right);

	LONG_PTR firstCh = tr.chrg.cpMin;
	HRGN rgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	//printf("TEXTRANGE %d %d\n", tr.chrg.cpMin, tr.chrg.cpMax);
	DWORD dwTxtSize = tr.chrg.cpMax - tr.chrg.cpMin;

	//CHAR* item;
	//char a[] = "apple sine";
	//item = strtok(a, " ");

	//while (item != NULL)
	//{
	//	printf("Tok : %s\n", item);
	//	item = strtok(NULL, " ");
	//}

	if (dwTxtSize > 0) {
		LPSTR buf = (LPSTR)malloc(sizeof(CHAR) * (dwTxtSize + 1));
		tr.lpstrText = buf;
		TCHAR buffer[256];
		//SendMessage(hwnd, EM_GETLINE, line, (LONG)&buffer);
		//strtok()
		//printf("line : %d text : %s\n", line, buffer);
		auto b = SendMessage(hwnd, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
		if (b > 0) {
			LPSTR start = tr.lpstrText;
			printf("%s\n", start);
			char* item;
			item = strtok(start, " ");
			int hlItem = 0;
			std::vector<char*> list;
			while (item != NULL)
			{
				//printf("Tok : %s\n", item);
				/*item = strtok(NULL, " ");*/
				//if (IsHighlightText(item)) {
				//	RECT txtRc;
				//	int range = item - tr.lpstrText + firstCh;
				//	printf("range : %d\n", range);
				//	SendMessage(hwnd, EM_POSFROMCHAR, (WPARAM)&txtRc, range);
				//	COLORREF old = SetTextColor(dc, RGB(175, 140, 220));
				//	SelectObject(dc, font);
				//	DrawText(dc, item, strlen(item), &txtRc, 0);
				//	hlItem++;
				//}
				if (strcmp(item, "int") == 0) {
					list.emplace_back(item);
				}
				
				
				item = strtok(NULL, " ");
			}
			printf("%d items hightlighted\n", list.size());
		}
		if (buf)
			free(buf);
	}
	SelectObject(dc, rgn);
	DeleteObject(rgn);
	ShowCaret(hwnd);
	return 0;

}