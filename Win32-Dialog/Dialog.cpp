#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <../Common/Log.h>
#include <../Common/Wnd32.h>

#define WC_WINDOW	"WC_WINDOW"
#define WC_CHILD	"WC_CHILD"
#define DLGTITLE  "Debug"
#define DLGFONT   "MS Sans Serif"
#define NUMCHARS(aa) (sizeof(aa)/sizeof((aa)[0]))
#define IDC_BITMAP 99

#include <DlgTemplate.h>

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
INT_PTR WINAPI DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);


struct WindowData
{
	HWND hwnd;
	LPCSTR name;
};

WindowData window;
WindowData dlg;

HFONT g_font;


int main(int args, char* argv[])
{
	InitCommonControls();

	WNDCLASSEX wc{};
	wc.cbSize = sizeof(wc);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = WC_WINDOW;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WndProc;
	GetClassInfoEx(wc.hInstance, wc.lpszClassName, &wc);
	RegisterClassEx(&wc);

	//Main window
	window.name = "Main Window";
	window.hwnd = CreateWindowEx(NULL, wc.lpszClassName, window.name,
		WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN, 600, 200, 700, 480, nullptr, 0, wc.hInstance, &window);


	DlgTemplateBase dlgBase("Memory Based Dialog", WS_VISIBLE | WS_POPUP, 20, 20, 280, 110);
	dlgBase.AddItem("&Apply", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
	DlgTemplateBase::Button, 200, 70, 66, 11, 1);
	dlgBase.AddItem("&Cancel", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		DlgTemplateBase::Button, 200, 90, 66, 11, 2);

	dlg.hwnd = CreateDialogIndirect(
		wc.hInstance,
		dlgBase,
		window.hwnd,
		//NULL,
		(DLGPROC)DlgProc,
		(LONG)&dlgBase);

	LOG::LogLastError();
	ShowWindow(window.hwnd, TRUE);
	
	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.lParam;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	return DefWindowProc(hwnd, msg, wp, lp);
}

void PaintContorl(HWND hwnd, HDC dc, const RECT& rc)
{
	SetBkMode(dc, TRANSPARENT);
	//DrawFillRect(dc, rc, RGB(100, 100, 100));
	auto pen = CreatePen(PS_SOLID, 1, RGB(20, 20, 20));
	auto br = CreateSolidBrush(RGB(65, 65, 65));
	SelectObject(dc, br);
	SelectObject(dc, pen);
	Rectangle(dc, rc.left, rc.top, rc.right, rc.bottom);


	SetTextColor(dc, RGB(200, 200, 200));
	LPCSTR lpText = Wnd32::GetHwndText(hwnd);
	SelectObject(dc, g_font);
	DrawText(dc, lpText, -1, (LPRECT)&rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	DeleteObject(pen);
	DeleteObject(br);
}
INT_PTR WINAPI DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	//printf("message : %s\n", LOG::WndMessage(msg));
	static RECT fbRc; //floating bar rect
	static HFONT font;
	switch (msg)
	{
	case WM_NCCREATE: {
		printf("WM_NCCREATE\n");
		break;
	}
	case WM_INITDIALOG: {
		printf("WM_INITDIALOG\n");
		HINSTANCE inst = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
		DWORD id = 0;
		DlgTemplateBase* base = (DlgTemplateBase*)lp;
		RECT rc;
		GetClientRect(hwnd, &rc);
		rc.bottom =  22;

		fbRc = rc;
		g_font = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, DLGFONT);
		break;
	}

	case WM_SETFONT: {
		RECT rc;
		GetClientRect(hwnd, &rc);
		printf("WM_FONT\n");
		ShowWindow(hwnd, SW_SHOW);
		break;
	}
	case WM_PAINT: {
		printf("WM_PAINT %s\n", Wnd32::GetHwndText(hwnd));
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);

		Wnd32::DrawFillRect(dc, ps.rcPaint, RGB(42, 42, 42));

		Wnd32::DrawFillRect(dc, fbRc, RGB(45, 60, 90));
		SetTextColor(dc, RGB(200, 200, 200));
		::SelectObject(dc, g_font);
		::DrawText(dc, Wnd32::GetHwndText(hwnd), -1, &fbRc, DT_SINGLELINE | DT_VCENTER | DT_WORDBREAK);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DRAWITEM: {

		LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lp;
		//printf("WM_DRAWITEM ID : %d NAME : %s\n", dis->CtlID, Wnd32::GetHwndText(dis->hwndItem));
		PaintContorl(dis->hwndItem, dis->hDC, dis->rcItem);
		break;
	}
	case WM_CLOSE: {
		DeleteObject(font);
		EndDialog(hwnd, IDCANCEL);
		break;
	}
	case WM_COMMAND: {
		HWND control = (HWND)lp;
		printf("Command : %s , id : %d\n", Wnd32::GetHwndText(control), GetDlgCtrlID(control));
		break;
	}
	
	case WM_NCHITTEST: {
		POINT pos;
		pos.x = (SHORT)LOWORD(lp);
		pos.y = (SHORT)HIWORD(lp);
		ScreenToClient(hwnd, &pos);
		bool bIn = PtInRect(&fbRc, pos);
		if (bIn) {
			SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		}
		break;
	}
	}
	//return DefDlgProc(hwnd, msg, wp, lp);
	return FALSE;
}
