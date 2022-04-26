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

#include <../Common/Wnd32.h>
#define WC_WINDOW	"WC_WINDOW"
#define WC_CHILD	"WC_CHILD"
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);


struct WindowData
{
	HWND hwnd;
	LPCSTR name;
};
#define BACK_BUFFER		0
#define FRONT_BUFFER	1
#define CLIENT_DC		0
#define MEMORY_DC		1
struct SwapChain{
	HBITMAP buffer[2];
	HDC dc[2];
};

WindowData window;
WindowData child;
SwapChain pSc;


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
		WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN, 600, 400, 700, 480, nullptr, 0, wc.hInstance, &window);

	wc.lpszClassName = WC_CHILD;
	RegisterClassEx(&wc);
	child.name = "Child Window";
	child.hwnd = CreateWindowEx(NULL, wc.lpszClassName, window.name,
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 120, 100, 200, 100, 
		window.hwnd, 0, wc.hInstance, &child);

	CreateWindowEx(NULL, wc.lpszClassName, "Second",
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 250, 100, 200, 200,
		window.hwnd, 0, wc.hInstance, &child);

	ShowWindow(window.hwnd, TRUE);

	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return msg.lParam;
}

void Paint(HDC dc, PAINTSTRUCT& ps) {
	HBRUSH br = CreateSolidBrush(RGB(20, 120, 220));
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(20, 20, 20));
	SelectObject(dc, br);
	SelectObject(dc, pen);
	RECT rc = ps.rcPaint;
	Rectangle(dc, rc.left, rc.top, rc.right, rc.bottom);
	DeleteObject(br);
	DeleteObject(pen);
}

void PaintDoubleBuffer(WindowData* wnd, PAINTSTRUCT& ps) {
	SIZE size{ ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top };
	POINT pos{ ps.rcPaint.left, ps.rcPaint.top };
	HDC memDc;
	HBITMAP hBmp;
	HBITMAP hOldBmp;
	POINT oldPos;
	memDc = CreateCompatibleDC(ps.hdc);
	hBmp = CreateCompatibleBitmap(ps.hdc, size.cx, size.cy);
	hOldBmp = (HBITMAP)SelectObject(memDc, hBmp);

	OffsetViewportOrgEx(memDc, -(pos.x), -(pos.y), &oldPos);
	Paint(memDc, ps);
	SetViewportOrgEx(memDc, oldPos.x, oldPos.y, NULL);
	//BitBlt(ps.hdc, pos.x, pos.y, ps.rcPaint.right, ps.rcPaint.bottom, memDc, 0, 0, SRCCOPY);
	BitBlt(ps.hdc, pos.x, pos.y, size.cx, size.cy, memDc, 0, 0, SRCCOPY);

	int mode = GetMapMode(memDc);
	MM_TEXT;
	

	SelectObject(memDc, hOldBmp);
	DeleteObject(hBmp);
	DeleteDC(memDc);

}
void ClearSwapChain(HWND hwnd, SwapChain* sc);
void SetSwapChain(HWND hwnd, SwapChain* sc, size_t w, size_t h) {
	ClearSwapChain(hwnd, sc);
	HDC dc = GetDC(hwnd);
	HDC memDc = CreateCompatibleDC(dc);
	HBITMAP front = CreateCompatibleBitmap(dc, w, h);
	HBITMAP back = CreateCompatibleBitmap(dc, w, h);
	sc->buffer[BACK_BUFFER] = back;
	sc->buffer[FRONT_BUFFER] = front;
	sc->dc[CLIENT_DC] = dc;
	sc->dc[MEMORY_DC] = memDc;
	GetMapMode(memDc);

	SelectObject(memDc, back);
	
}

void ClearSwapChain(HWND hwnd, SwapChain* sc) {
	DeleteObject(sc->buffer[0]);
	DeleteObject(sc->buffer[1]);
	ReleaseDC(hwnd, sc->dc[0]);
	DeleteDC(sc->dc[1]);
}

void DoPaint(SwapChain* sc, HWND hwnd) {
	RECT rc;
	GetClientRect(hwnd, &rc);
	Wnd32::DrawFillRect(sc->dc[1], rc, RGB(100, 0, 0));
}

void UpdateSwapChain(SwapChain* sc, const RECT& rc) {
	BitBlt(sc->dc[0], rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
		sc->dc[1], 0, 0, SRCCOPY);
	HBITMAP swapBuffer = sc->buffer[FRONT_BUFFER];
	sc->buffer[FRONT_BUFFER] = sc->buffer[BACK_BUFFER];
	sc->buffer[BACK_BUFFER] = swapBuffer;
	(HBITMAP)SelectObject(sc->dc[MEMORY_DC], sc->buffer[FRONT_BUFFER]);
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	RECT rc;
	TCHAR code[256];
	
	WindowData* wnd = (WindowData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	BOOL moveAble = FALSE;
	switch (msg)
	{
	case WM_NCCREATE:
	{
		wnd = (WindowData*)((LPCREATESTRUCT)lp)->lpCreateParams;
		if (!wnd) break;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)wnd);
		printf("[WM_NCCREATE] %s\n", wnd->name);
		break;
	}
	case WM_PAINT: {
		if (wnd == &window) break;
		printf("[WM_PAINT] %s\n", wnd->name);
		//PAINTSTRUCT ps{};
		//HDC dc = BeginPaint(hwnd, &ps);
		DoPaint(&pSc, hwnd);
		RECT rc;
		GetClientRect(hwnd, &rc);
		UpdateSwapChain(&pSc, rc);
		//EndPaint(hwnd, &ps);
		break;
	}
	case WM_SIZE: {
		if (wnd == &window) break;
		printf("WM_SIZE %s\n", Wnd32::GetHwndText(hwnd));
		SetSwapChain(hwnd, &pSc, LOWORD(lp), HIWORD(lp));
		//InvalidateRect(hwnd, NULL, FALSE);
		break;
	}
	case WM_LBUTTONDOWN: {
		InvalidateRect(hwnd, NULL, FALSE);
	}
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}




