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

#define WC_WINDOW	"WC_WINDOW"
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT WINAPI SubClassWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR id, DWORD_PTR data);
LRESULT WINAPI HookCallWndProc(INT code, WPARAM wp, LPARAM lp);

HHOOK hHook = nullptr;

struct WindowData
{
	HWND hwnd;
	LPCSTR name;
};

WindowData window;
WindowData button_subclass;
WindowData button_subclass_hook;

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
		WS_POPUP | WS_VISIBLE, 600, 400, 700, 300, nullptr, 0, wc.hInstance, &window);

	//Basic SubClassing
	//
	button_subclass.name = "SubClass Button";
	button_subclass.hwnd = CreateWindowEx(NULL, WC_BUTTON, button_subclass.name,
		WS_CHILD | WS_VISIBLE, 50, 120, 200, 60, window.hwnd, 0, wc.hInstance, &button_subclass);
	SetWindowSubclass(button_subclass.hwnd, SubClassWndProc, 0, 0);
	//SubClassing with HOOK
	//Begin Hook(to get WM_NCCREATE WM_NCSIZE WM_NCPAINT WM_CREATE WM_SIZE)
	hHook = SetWindowsHookEx(WH_CALLWNDPROC, HookCallWndProc, nullptr, GetCurrentThreadId());
	button_subclass_hook.name = "SubClass Hook Button";
	button_subclass_hook.hwnd = CreateWindowEx(NULL, WC_BUTTON, button_subclass_hook.name,
		WS_CHILD | WS_VISIBLE, 350, 120, 200, 60, window.hwnd, 0, wc.hInstance, &button_subclass_hook);
	//End of Hook
	UnhookWindowsHookEx(hHook);
	hHook = nullptr;
	SetWindowSubclass(button_subclass_hook.hwnd, SubClassWndProc, 0, 0);

	ShowWindow(window.hwnd, TRUE);
	
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
	RECT rc;
	TCHAR code[256];
	WindowData* window = nullptr;
	switch (msg)
	{
	case WM_NCCREATE:
	{
		window = (WindowData*)((LPCREATESTRUCT)lp)->lpCreateParams;
		if (!window) break;
		printf("[WM_NCCREATE] %s\n", window->name);
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rc);
		HBRUSH br = (HBRUSH)GetStockObject(DC_BRUSH);
		SelectObject(dc,br);
		if (hwnd == button_subclass.hwnd) {
			SetDCBrushColor(dc, RGB(120, 80, 0));
		}
		else if (hwnd == button_subclass_hook.hwnd) {
			SetDCBrushColor(dc, RGB(50, 80, 150));
		}
		else {
			SetDCBrushColor(dc, RGB(60, 60, 60));
		}
		FillRect(dc, &rc, br);
		
		GetWindowText(hwnd, code, 256);
		SetBkMode(dc, TRANSPARENT);
		SetTextColor(dc, RGB(220, 220, 220));
		if(hwnd == button_subclass.hwnd || hwnd == button_subclass_hook.hwnd)
			DrawText(dc, code, strlen(code), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		EndPaint(hwnd, &ps);
		break;
	}
	}

	if (hwnd == button_subclass.hwnd || hwnd == button_subclass_hook.hwnd) {
		return DefSubclassProc(hwnd, msg, wp, lp);
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT WINAPI SubClassWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR id, DWORD_PTR data)
{
	return WndProc(hwnd, msg, wp, lp);
}

LRESULT WINAPI HookCallWndProc(INT code, WPARAM wp, LPARAM lp)
{
	if(code < 0)
		return CallNextHookEx(hHook, code, wp, lp);
	if (code == HC_ACTION) {
		CWPSTRUCT* cs = (CWPSTRUCT*)lp;
		LRESULT res = WndProc(cs->hwnd, cs->message, cs->wParam, cs->lParam);
	}
	return CallNextHookEx(hHook, code, wp, lp);
}

