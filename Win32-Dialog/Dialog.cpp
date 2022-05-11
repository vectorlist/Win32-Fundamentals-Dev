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
#define DLGAPPLY  "&Apply"
#define DLGCANCEL "&Cancel"
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


	DlgTemplateBase dlgBase("Memory Dialog", WS_VISIBLE | WS_POPUPWINDOW, 20, 20, 200, 100);
	dlgBase.AddItem("Memory Ctrl", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
	DlgTemplateBase::Button, 0, 0, 100, 40, 1);

	dlg.hwnd = CreateDialogIndirect(wc.hInstance, dlgBase,
		window.hwnd,(DLGPROC)DlgProc, (LONG)&dlgBase);

	LOG::LogLastError();
	ShowWindow(window.hwnd, TRUE);
	//ShowWindow(dlg.hwnd, TRUE);

	
	
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

LPCSTR GetWindowText(HWND hwnd) {
	static std::string szTempStr;
	UINT len = GetWindowTextLength(hwnd);
	szTempStr.reserve(len);
	GetWindowText(hwnd, (LPSTR)szTempStr.c_str(), len);
	return szTempStr.c_str();
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


	SetTextColor(dc, RGB(220, 220, 220));
	LPCSTR lpText = GetWindowText(hwnd);

	DrawText(dc, lpText, -1, (LPRECT)&rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	DeleteObject(pen);
	DeleteObject(br);
}
INT_PTR WINAPI DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	//printf("message : %s\n", LOG::WndMessage(msg));
	switch (msg)
	{
	case WM_NCCREATE: {
		printf("WM_NCCREATE\n");
		break;
	}
	case WM_INITDIALOG: {
		printf("WM_INITDIALOG\n");
		HINSTANCE inst = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
		HWND button = CreateWindowEx(NULL, WC_BUTTON, "button", WS_VISIBLE | WS_CHILD,
			180, 20, 120, 30, hwnd,
			(HMENU)33, inst, NULL);
		DWORD id = 0;
		DlgTemplateBase* base = (DlgTemplateBase*)lp;

		//UINT size = base->m_items.size();
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

		Wnd32::DrawFillRect(dc, ps.rcPaint, RGB(35, 35, 35));
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DRAWITEM: {
		
		LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lp;
		printf("id : drawItem %d", dis->CtlID);
		PaintContorl(dis->hwndItem,dis->hDC, dis->rcItem);
		break;
	}
	case WM_CLOSE: {
		EndDialog(hwnd, IDCANCEL);
		break;
	}
	case WM_NCHITTEST: {
		HRESULT hit = DefWindowProc(hwnd, msg, wp, lp);
		if (hit == HTCLIENT) {
			return HTCAPTION;
		}
		break;
	}
	}
	//return DefDlgProc(hwnd, msg, wp, lp);
	return FALSE;
}
