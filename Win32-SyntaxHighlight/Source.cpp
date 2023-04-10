#define no_init_all
#define UNICODE
#define _UNICODE


#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <richedit.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Code::Block doesn't have a tom.h :(
// https://github.com/kinke/mingw-w64-crt/blob/master/mingw-w64-headers/include/tom.h
#include "tom.h" 
#include <richole.h>
#include <unknwn.h>

IUnknown *tr_code = NULL;
ITextDocument *td_code;

#define DEFINE_GUIDXXX(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) EXTERN_C const GUID name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
DEFINE_GUIDXXX(IID_ITextDocument, 0x8CC497C0, 0xA1DF, 0x11CE, 0x80, 0x98, 0x00, 0xAA, 0x00, 0x47, 0xBE, 0x5D);
//DEFINE_GUIDXXX(IID_ITextDocument, 0x8CC497C0, 0xA1DF, 0x11CE, 0x80, 0x98, 0x00, 0xAA, 0x00, 0x47, 0xBE, "hello");

#define IDC_EDITOR 1000
#define IDC_BUTTON 1001

HWND hEditorWnd;

bool updateHighlighting(HWND hWnd) {
	int carriagePos = 0;
	SendMessage(hWnd, EM_GETSEL, (WPARAM)&carriagePos, (WPARAM)&carriagePos);

	CHARFORMAT cf = { 0 };
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR | CFM_BOLD;
	cf.crTextColor = RGB(0, 0, 200);
	cf.dwEffects = CFM_BOLD;
	SendMessage(hWnd, EM_SETSEL, 0, -1);
	SendMessage(hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);

	cf.dwMask = CFM_COLOR | CFM_BOLD;
	cf.dwEffects = cf.dwEffects & ~CFM_BOLD;
	cf.crTextColor = RGB(200, 0, 0);
	SendMessage(hWnd, EM_SETSEL, 4, 6);

	SendMessage(hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	SendMessage(hWnd, EM_SETSEL, carriagePos, carriagePos);

	_tprintf(TEXT("End highlight\n"));
	return true;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_COMMAND: {
		if (LOWORD(wParam) == IDC_BUTTON && HIWORD(wParam) == BN_CLICKED)
			_tprintf(TEXT("UNDO: can %i => result: %i\n"), Edit_CanUndo(hEditorWnd), Edit_Undo(hEditorWnd));

		if (LOWORD(wParam) == IDC_EDITOR && HIWORD(wParam) == EN_CHANGE) {
			long cnt = 0;
			td_code->Undo(tomSuspend, NULL); // <---
			td_code->Freeze(&cnt);           // <---
			updateHighlighting(hEditorWnd);
			td_code->Unfreeze(&cnt);         // <---
			td_code->Undo(tomResume, NULL);  // <---
			return 1;
		}

	}
					 break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

int main(int args, char* argv[]) {
	InitCommonControls();
	LoadLibrary(TEXT("msftedit.dll"));

	HWND hWnd;
	MSG msg;
	WNDCLASSEX wincl;

	auto hInst = GetModuleHandle(NULL);

	wincl.hInstance = hInst;
	wincl.lpszClassName = TEXT("AppClass");
	wincl.lpfnWndProc = WindowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof(WNDCLASSEX);
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

	if (!RegisterClassEx(&wincl))
		return 0;

	hWnd = CreateWindowEx(0, TEXT("AppClass"), TEXT("Test"), WS_OVERLAPPEDWINDOW | WS_VISIBLE, 300, 400, 310, 375,
		HWND_DESKTOP, NULL, hInst, NULL);

	CreateWindowEx(0, WC_BUTTON, L"UNDO", WS_VISIBLE | WS_CHILD | WS_TABSTOP,
		10, 310, 280, 30, hWnd, (HMENU)IDC_BUTTON, hInst, NULL);
	hEditorWnd = CreateWindowEx(0, TEXT("RICHEDIT50W"), NULL, WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | ES_NOHIDESEL,
		0, 0, 300, 300, hWnd, (HMENU)IDC_EDITOR, hInst, NULL);
	SendMessage(hEditorWnd, EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_UPDATE | ENM_KEYEVENTS);

	// \/   
	SendMessage(hEditorWnd, EM_GETOLEINTERFACE, 0, (LPARAM)&tr_code);
	if (tr_code == (IRichEditOle*)NULL) {
		MessageBox(0, TEXT("Error when trying to get RichEdit OLE Object"), NULL, 0);
		return 0;
	}
	tr_code->QueryInterface(IID_ITextDocument, (void**)&td_code);
	// /\    

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}