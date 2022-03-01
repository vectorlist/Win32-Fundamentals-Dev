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
#define WC_CHILD	"WC_CHILD"
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
HWND mainHwnd;
HWND subclassControl;
HWND subclassHookControl;

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

	mainHwnd = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW, WS_POPUP | WS_VISIBLE, 600, 400, 700, 300, nullptr, 0, wc.hInstance, NULL);

	wc.lpszClassName = WC_BUTTON;
	if (!GetClassInfoEx(wc.hInstance, wc.lpszClassName, &wc)) {
		printf("%s is built-in class... set subclass", wc.lpszClassName);
	}

	subclassControl = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW, WS_CHILD | WS_VISIBLE, 50, 120, 100, 60, mainHwnd, 0, wc.hInstance, NULL);

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
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
