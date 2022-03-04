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

#define WC_WINDOW "Window"
#define WC_SUPERCLASS_BUTTON "Superclass Button"
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
WNDPROC pPreWndProc = nullptr;
struct WindowData
{
	HWND hwnd;
	LPCSTR name;
};

WindowData window;
WindowData button;
WindowData sc_button;

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
	pPreWndProc = swc.lpfnWndProc;
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
	sc_button.name = WC_SUPERCLASS_BUTTON;
	sc_button.hwnd = CreateWindowEx(NULL, swc.lpszClassName, sc_button.name,
		BS_PUSHBUTTON |WS_VISIBLE | WS_CHILD,
		70, 100, 200, 60, window.hwnd, 0, swc.hInstance, &sc_button);
	//creating basic button control
	button.name = WC_BUTTON;
	button.hwnd = CreateWindowEx(NULL, WC_BUTTON, button.name,
		BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD,
		300, 100, 200, 60, window.hwnd, 0, swc.hInstance, &button);

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
		if (hwnd == sc_button.hwnd) {
			SetDCBrushColor(dc, RGB(120, 80, 0));
		}
		else {
			SetDCBrushColor(dc, RGB(60, 60, 60));
		}
		FillRect(dc, &rc, br);

		GetWindowText(hwnd, code, 256);
		printf("[WM_PAINT] %s\n", code);
		SetBkMode(dc, TRANSPARENT);
		SetTextColor(dc, RGB(220, 220, 220));
		if (hwnd == sc_button.hwnd)
			DrawText(dc, code, strlen(code), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		EndPaint(hwnd, &ps);
		break;
	}
	}
	if (hwnd == sc_button.hwnd) {

		return CallWindowProc(pPreWndProc, hwnd, msg, wp, lp);
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}
