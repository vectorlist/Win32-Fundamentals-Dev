#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <assert.h>
#include <../Common/Wnd32.h>

#define WC_MAINWINDOW	"MAINWINDOW"
#define WC_SUB		"SUB_WINDOW"

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT WINAPI SubProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
HWND mainHwnd;
HWND subHwnd;

int main(int args, char* argv[])
{
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = WC_MAINWINDOW;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WndProc;
	assert(RegisterClassEx(&wc));
	wc.lpszClassName = WC_SUB;
	wc.lpfnWndProc = SubProc;
	assert(RegisterClassEx(&wc));

	int w = 780;
	int h = 480;
	int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

	mainHwnd = CreateWindowEx(NULL, WC_MAINWINDOW, "main window",
		WS_VISIBLE | WS_POPUP | WS_SIZEBOX,
		x, y, w, h, nullptr, (HMENU)0, wc.hInstance, 0);

	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.lParam;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg)
	{
	case WM_NCCREATE: {
		HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
		
		subHwnd = CreateWindowEx(0, WC_SUB, WC_SUB, WS_VISIBLE | WS_CHILD, 0, 0, 0, 0,
			hwnd, 0, hInst, 0);
	}break;
	case WM_SIZE: {
		RECT rc;
		GetClientRect(hwnd, &rc);
		InflateRect(&rc, -20, -20);
		Wnd32::SetGeometry(subHwnd, rc);
		break;
	}break;
	case WM_KEYDOWN: {
		if (wp == VK_ESCAPE) {
			DestroyWindow(hwnd);
		}
	}break;
	case WM_CLOSE: {
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		Wnd32::DrawFillRect(dc, ps.rcPaint, RGB(100, 0, 0));
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_GETMINMAXINFO: {
		MINMAXINFO* info = (LPMINMAXINFO)lp;
		info->ptMaxTrackSize = POINT{ 600,600 };
		info->ptMinTrackSize = POINT{ 200,200 };
		break;
	}
	case WM_ERASEBKGND: {
		return TRUE;
	}
	case WM_NCDESTROY: {
		PostQuitMessage(0);
	}break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT WINAPI SubProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg)
	{
	case WM_PAINT: {
		//bool b = GetUpdateRect(hwnd, NULL, TRUE);
		//printf("WM_PAINT %s %d\n", Wnd32::GetHwndText(hwnd),b);
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		Pen pen(1, RGB(120, 120, 120));
		Brush br(RGB(42, 42, 42));
		SelectObject(dc, pen);
		SelectObject(dc, br);
		RECT rc;
		GetClientRect(hwnd, &rc);
		Wnd32::DrawRoundRect(dc, rc, 8, 8);
		SetTextColor(dc, RGB(200, 200, 200));
		SetBkMode(dc, TRANSPARENT);
		printf("%d\n", rc.right);
		DrawText(dc, Wnd32::GetHwndText(hwnd), -1, &rc, DT_CENTER | DT_SINGLELINE);
		EndPaint(hwnd, &ps);
		break;
	}break;
	case WM_ERASEBKGND: {
		return TRUE;
		break;
	}
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}