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

#define WC_WINDOW "WC_WINDOW"
#define NONCLIENT_THICKNESS 20
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
WNDPROC pPreWndProc = nullptr;
struct WindowData
{
	HWND hwnd;
	LPCSTR name;
};

WindowData window;
WindowData button;

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
	window.name = WC_WINDOW;
	window.hwnd = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW, WS_POPUP | WS_VISIBLE,
		500, 200, 700, 300, nullptr, 0, wc.hInstance, &window);

	//SuperClassing
	//Get Control Base infomation
	WNDCLASSEX swc{};
	swc.cbSize = sizeof(WNDCLASSEX);

	bool bInfo = GetClassInfoEx(GetModuleHandle(NULL), WC_BUTTON, &swc);

	pPreWndProc = swc.lpfnWndProc;
	swc.cbWndExtra += sizeof(WindowData*);
	swc.hInstance = GetModuleHandle(NULL);
	swc.lpszClassName = "new button";
	swc.style &= ~CS_GLOBALCLASS;
	swc.lpfnWndProc = WndProc;
	
	std::cout << LOG::ClassStyle(swc.style) << std::endl;

	bool b = RegisterClassEx(&swc);
	if (b)
		printf("Register Class %s\n", swc.lpszClassName);

	button.name = "button_ex";
	button.hwnd = CreateWindowEx(NULL, swc.lpszClassName, button.name,
		BS_PUSHBUTTON |WS_VISIBLE | WS_CHILD,
		100, 100, 30, 100, window.hwnd, 0, swc.hInstance, &button);

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
	switch (msg)
	{
	case WM_NCCREATE: {
		WindowData* window = (WindowData*)((LPCREATESTRUCT)lp)->lpCreateParams;
		printf("create %s\n", window->name);
		break; 
	}
	}
	if (hwnd == button.hwnd) {

		return CallWindowProc(pPreWndProc, hwnd, msg, wp, lp);
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
