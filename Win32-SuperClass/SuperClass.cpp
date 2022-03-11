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

#include <Log.h>

#define WC_WINDOW				"Window"
#define WC_SUBCLASS_BUTTON		"Subclass Button"
#define WC_SUPERCLASS_BUTTON	"Superclass Button"

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

WNDPROC pPreSubClassWndProc = nullptr;
WNDPROC pPreSuperClassWndProc = nullptr;

struct WindowData
{
	HWND hwnd;
	LPCSTR name;
};

WindowData window;
WindowData sub_button;
WindowData super_button;

int main(int args, char* argv[])
{
	//initialize controls
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
	window.name = WC_WINDOW;
	window.hwnd = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW, WS_POPUP | WS_VISIBLE,
		700, 300, 700, 300, nullptr, 0, wc.hInstance, &window);

	//SuperClassing
	//Get Control Base infomation
	WNDCLASSEX swc{};
	swc.cbSize = sizeof(WNDCLASSEX);

	if (GetClassInfoEx(NULL, WC_BUTTON, &swc)) {
		printf("Copy WC_BUTTON info to WC_SUPERCLASS_BUTTON\n");
	}
	//temportary save wndproc
	pPreSuperClassWndProc = swc.lpfnWndProc;
	//to use SetWindowLong or SetWindowLongPtr(hwnd, 0, data)
	swc.cbWndExtra += sizeof(WindowData*);
	swc.hInstance = GetModuleHandle(NULL);
	swc.lpszClassName = WC_SUPERCLASS_BUTTON;
	//Remove Internal register SuperClass
	swc.style &= ~CS_GLOBALCLASS;
	//set new wndproc
	swc.lpfnWndProc = WndProc;
	
	std::cout << LOG::ClassStyle(swc.style) << std::endl;

	//has aldreay new superclass if it failed
	if (RegisterClassEx(&swc)) {
		printf("registered new %s\n", swc.lpszClassName);
	}
	//creating new superclass control
	super_button.name = WC_SUPERCLASS_BUTTON;
	super_button.hwnd = CreateWindowEx(NULL, swc.lpszClassName, super_button.name,
		BS_PUSHBUTTON |WS_VISIBLE | WS_CHILD,
		70, 100, 200, 60, window.hwnd, 0, swc.hInstance, &super_button);
	//creating button control
	sub_button.name = WC_SUBCLASS_BUTTON;
	sub_button.hwnd = CreateWindowEx(NULL, WC_BUTTON, sub_button.name,
		BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD,
		300, 100, 200, 60, window.hwnd, 0, swc.hInstance, &sub_button);
	//set classic subclassing 
	pPreSubClassWndProc = (WNDPROC)SetWindowLongPtr(sub_button.hwnd, GWLP_WNDPROC, (LONG)WndProc);

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
	TCHAR code[125];
	switch (msg)
	{
	case WM_NCCREATE: {
		WindowData* window = (WindowData*)((LPCREATESTRUCT)lp)->lpCreateParams;
		printf("[WM_NCCREATE] %s\n", window->name);
		break; 
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rc);
		HBRUSH br = (HBRUSH)GetStockObject(DC_BRUSH);
		SelectObject(dc, br);
		if (hwnd == super_button.hwnd) {
			SetDCBrushColor(dc, RGB(120, 80, 0));
		}
		else if(hwnd == sub_button.hwnd) {
			SetDCBrushColor(dc, RGB(60, 120, 180));
		}
		else {
			SetDCBrushColor(dc, RGB(60, 60, 60));
		}
		FillRect(dc, &rc, br);
		if (hwnd != window.hwnd) {
			GetWindowText(hwnd, code, 125);
			printf("[WM_PAINT] %s\n", code);
			SetBkMode(dc, TRANSPARENT);
			SetTextColor(dc, RGB(220, 220, 220));
			DrawText(dc, code, strlen(code), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		EndPaint(hwnd, &ps);
		break;
	}
	}
	if (hwnd == super_button.hwnd) {
		return CallWindowProc(pPreSuperClassWndProc, hwnd, msg, wp, lp);
	}
	if (hwnd == sub_button.hwnd) {
		return CallWindowProc(pPreSubClassWndProc, hwnd, msg, wp, lp);
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

