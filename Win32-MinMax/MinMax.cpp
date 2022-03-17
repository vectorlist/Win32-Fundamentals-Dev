#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>

#define WC_WINDOW	"WC_WINDOW"
#define WC_CHILD	"WC_CHILD"
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
bool bTrackMouse = false;
bool bEnteredMouse = false;
HWND mainHwnd;
HWND childHwnd;

int main(int args, char* argv[])
{
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

	mainHwnd = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW, WS_POPUP | WS_VISIBLE, 500, 200, 700, 300, nullptr, 0, wc.hInstance, NULL);

	wc.lpszClassName = WC_CHILD;
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	RegisterClassEx(&wc);

	childHwnd = CreateWindowEx(NULL, wc.lpszClassName, "Button", WS_CHILD | WS_VISIBLE, 170, 80, 200, 50, mainHwnd, 0, wc.hInstance, NULL);
	ShowWindow(mainHwnd, TRUE);
	SendMessage(mainHwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
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
	switch (msg)
	{
	case WM_CREATE: {
		break;
	}
	case WM_SYSCOMMAND: {
		printf("WM_SYSCOMMAND\n");
		switch (wp)
		{
		case SC_CLOSE: {
			break;
		}
		case SC_MAXIMIZE: {
			break;
		}
		case SC_MINIMIZE: {
			break;
		}
		case SC_RESTORE: {
			printf("WM_SYSCOMMAND : SC_RESTORE\n");
			break;
		}
		}
		break;
	}
	case WM_PAINT: {
		break;
	}

	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
