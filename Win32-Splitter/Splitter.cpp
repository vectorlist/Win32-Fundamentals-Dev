#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>

#define WC_WINDOW	"WC_WINDOW"
#define WIDTH 840
#define HEIGHT 480
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
HWND hMain;
HWND hEdit[2];
HCURSOR hCursor;

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

	int x = (GetSystemMetrics(SM_CXSCREEN) - WIDTH) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - HEIGHT) / 2;
	hMain = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW, WS_POPUP | WS_VISIBLE,
		x, y, WIDTH, HEIGHT, nullptr, 0, wc.hInstance, NULL);
	
	ShowWindow(hMain, TRUE);
	UpdateWindow(hMain);

	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.lParam;
}

void DrawFillRect(HDC dc, const RECT &rc, COLORREF clr) {
	::SetBkColor(dc, clr);
	::ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
}

void OnDraw(HWND hwnd, HDC dc) {
	RECT rc;
	GetClientRect(hwnd, &rc);
	DrawFillRect(dc, rc, RGB(100, 0, 0));
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	RECT rc{0};
	TCHAR code[256];
	HINSTANCE inst = nullptr;
	
	
	static bool bSplitterMoving;
	static DWORD dwPos;
	switch (msg)
	{
	case WM_CREATE: {
		inst = ((LPCREATESTRUCT)lp)->hInstance;
		hEdit[0] = CreateWindowEx(NULL, "edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | ES_MULTILINE | WS_VSCROLL,
			0, 0, 0, 0, hwnd, (HMENU)1, inst, NULL);
		hEdit[1] = CreateWindowEx(NULL, "edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | ES_MULTILINE | WS_VSCROLL,
			0, 0, 0, 0, hwnd, (HMENU)2, inst, NULL);
		//RectVisible(hwnd, nullptr);
		//DPtoLP()
		hCursor = LoadCursor(NULL, IDC_SIZENS);
		bSplitterMoving = false;
		dwPos = 130;
		break;
	}
	case WM_SIZE:
	{
		if (HIWORD(lp) < dwPos) {
			dwPos = HIWORD(lp) - 10;
		}
		MoveWindow(hEdit[0], 0, 0, LOWORD(lp), dwPos - 1, TRUE);
		MoveWindow(hEdit[1], 0, dwPos + 2, LOWORD(lp), HIWORD(lp) - dwPos - 2, TRUE);
		return 0;
		break;
	}
	case WM_MOUSEMOVE:
	{
		POINT pos{ LOWORD(lp), HIWORD(lp) };
		if (pos.y > 10) {
			SetCursor(hCursor);
			if (wp == MK_LBUTTON && bSplitterMoving) {
				GetClientRect(hwnd, &rc);
				if (pos.y > rc.bottom) return 0;
			}
			dwPos = pos.y;
			SendMessage(hwnd, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));
		}
		return 0;
		break;
	}
	case WM_LBUTTONDOWN:
	{
		SetCursor(hCursor);
		bSplitterMoving = true;
		SetCapture(hwnd);
		break;
	}
	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		bSplitterMoving = false;
		printf("up\n");
		InvalidateRect(hwnd, nullptr, TRUE);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		HDC dc = nullptr;
		//Get Invalid area 
		//BOOL bInvalide = GetUpdateRect(hwnd, nullptr, FALSE);
		BOOL bInvalide = DefWindowProc(hwnd, msg, wp, lp);
		if (bInvalide) {
			printf("WM_PAINT BeginPaint\n");
			dc = BeginPaint(hwnd, &ps);
			OnDraw(hwnd, dc);
			EndPaint(hwnd, &ps);
		}
		else {
			printf("WM_PAINT DC paint\n");
			dc = GetDC(hwnd);
			OnDraw(hwnd, dc);
			ReleaseDC(hwnd, dc);
		}
		
		return 0;
		break;
	}
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}