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


struct WindowData
{
	HWND hwnd;
	LPCSTR name;
};

WindowData window;
WindowData child;


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
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 120, 100, 400, 200, window.hwnd, 0, wc.hInstance, &child);

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

void MapToParent(HWND hwnd, POINT in) {
	RECT rc[2];
	HWND parent = GetParent(hwnd);
	GetWindowRect(parent, &rc[0]);
	GetWindowRect(hwnd, &rc[1]);

	POINT offset{ rc[0].left - rc[1].left, rc[0].top - rc[1].top };
	printf("offset %d %d\n", offset.x, offset.y);
}

RECT MapToParentRect(HWND hwnd) {
	RECT rc;
	GetWindowRect(hwnd, &rc);
	::MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&rc, 2);
	return rc;
}

POINT pos[2] = { 0 };

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
		//printf("[WM_PAINT] %s\n", wnd->name);
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		PaintDoubleBuffer(wnd, ps);
		//Paint(dc, ps);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_LBUTTONDOWN: {
		if (wnd == &window) break;
		GetCursorPos(&pos[0]);
		ScreenToClient(GetParent(hwnd), &pos[0]);
		printf("Last Pos : %d %d\n", pos[0].x, pos[0].y);
		//SetCapture(hwnd);
		moveAble = TRUE;
		break;
	}
	case WM_LBUTTONUP: {
		if (wnd == &window) break;
		GetCursorPos(&pos[1]);
		
		printf("New Pos : %d %d\n", pos[1].x, pos[1].y);
		//ReleaseCapture();
		moveAble = FALSE;
		InvalidateRect(hwnd, NULL, FALSE);
		break;
	}
	case WM_MOUSEMOVE: {
		//if (wnd == &window) break;
		POINT curPos{ LOWORD(lp), HIWORD(lp) };
		//ClientToScreen(hwnd, &curPos);
		printf("Position \n");
		if ((wnd == &child)) {
			//if (!moveAble) break;
			if (wp != VK_LBUTTON) {
				//printf("BK_LBUTTON\n");
				break;
			}
			POINT offset;
			POINT clientPos;
			GetCursorPos(&clientPos);

			POINT parentCoord = clientPos;
			ScreenToClient(GetParent(hwnd), &parentCoord);
			ScreenToClient(hwnd, &clientPos);

			POINT oriPos = pos[0];
			int x,y;
			x = oriPos.x;
			y = oriPos.y;
			//if (oriPos.x < parentCoord.x) {
			//	x -= parentCoord.x;
			//}
			//else {
			//	x -= parentCoord.x;
			//}
			x -= parentCoord.x;
			y -= parentCoord.y;

			//RECT rc = MapToParentRect(hwnd);
			RECT rc;
			GetClientRect(hwnd, &rc);

			OffsetRect(&rc, -x, -y);
			printf("ori %d client %d offset %d\n", oriPos.x, parentCoord.x, -x);
			//printf("parent map %d %d\n", x, pos[0].y);
			

			//MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
			//SetWindowPos(hwnd, 0, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED);
			//UpdateWindow(hwnd);
			//InvalidateRect(hwnd, NULL, TRUE);
		}
		break;
	}
	case WM_NCHITTEST: {
		if (wnd == &child) {
			InvalidateRect(hwnd, NULL, FALSE);
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				return HTCAPTION;
			}
			
		}
			
		break;
	}
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}




