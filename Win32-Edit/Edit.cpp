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

#define WC_WINDOW	"WC_WINDOW"

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

HWND mainHwnd;

HWND edit;
HWND button;
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
		WS_POPUP | WS_VISIBLE, 600, 400, 760, 480, nullptr, 0, wc.hInstance, NULL);

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

	//LoadLibrary(TEXT("Msftedit.dll"));
	edit = CreateWindowEx(NULL, WC_EDIT, "Text Here...",
		ES_MULTILINE | ES_AUTOVSCROLL |WS_VISIBLE | WS_CHILD ,
		10, 10, 550, 360, mainHwnd, (HMENU)0, hInst, NULL);

	button = CreateWindowEx(NULL, WC_BUTTON, "Get Text",
		 WS_VISIBLE | WS_CHILD,
		600, 200, 110, 30, mainHwnd, (HMENU)0, hInst, NULL);

	SendMessage(edit, WM_SETFONT, (WPARAM)pObj[2], 0);

	ShowWindow(mainHwnd, TRUE);

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
	switch (msg)
	{
	case WM_SETFONT: {
		printf("WM_SETFONT\n");
		break;
	}
	case WM_COMMAND: {
		if ((HWND)lp == button) {
			printf(Wnd32::GetHwndText(edit));
			Sleep(1000);
		}
		break;
	}
	case WM_CTLCOLOREDIT: {
		HDC dc = (HDC)wp;
		SetTextColor(dc, RGB(220, 220, 220));
		::SetBkMode(dc, OPAQUE);
		SetBkColor(dc, RGB(33, 33, 33));
		return (LRESULT)pObj[0];
	}
	}
	
	return DefWindowProc(hwnd, msg, wp, lp);
}

