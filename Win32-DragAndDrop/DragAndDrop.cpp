#define no_init_all
//#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>

#define WC_WINDOW "MainWindow"
#define WIDTH 780
#define HEIGHT 420

RECT drop_rc;
std::string files;

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

int main(int args, char* argv[])
{
	HINSTANCE hInst  = GetModuleHandle(NULL);
	WNDCLASSEX wc{};
	wc.cbSize        = sizeof(wc);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hInstance     = hInst;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = WC_WINDOW;
	wc.style         = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc   = WndProc;
	GetClassInfoEx(wc.hInstance, wc.lpszClassName, &wc);
	RegisterClassEx(&wc);

	int x = (GetSystemMetrics(SM_CXSCREEN) - WIDTH) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - HEIGHT) / 2;
	
	
	DWORD styleEx = WS_EX_ACCEPTFILES;
	//DWORD styleEx = NULL;
	HWND hwnd = CreateWindowEx(styleEx, wc.lpszClassName, wc.lpszClassName,
		WS_VISIBLE | WS_POPUP, x, y, WIDTH, HEIGHT, nullptr, 0, hInst, 0);

	//DragAcceptFiles(hwnd, TRUE);

	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void OnPaint(HDC dc) {
	HWND hwnd = ::WindowFromDC(dc);
	RECT rc, top_rc;
	GetClientRect(hwnd, &rc);
	top_rc = rc;
	top_rc.bottom = 120;
	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, RGB(200, 200, 200));
	::DrawText(dc, "drop files here...", -1, &top_rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	HBRUSH br = (HBRUSH)GetStockObject(DC_BRUSH);
	SetDCBrushColor(dc, RGB(80, 88, 90));
	SelectObject(dc, br);
	Rectangle(dc, drop_rc.left, drop_rc.top, drop_rc.right, drop_rc.bottom);

	if (files.size() > 0) {
		rc = drop_rc;
		InflateRect(&rc, -5, -5);
		::DrawText(dc, (LPSTR)files.data(), -1, &rc, DT_NOCLIP | DT_NOPREFIX | DT_WORDBREAK);
	}
}

void OnDropFile(HWND hwnd, WPARAM wp) {
	CHAR buffer[256];
	HDROP hd = (HDROP)wp;
	POINT pt;
	DragQueryPoint(hd, &pt);
	bool bDrop = PtInRect(&drop_rc, pt);
	if (!bDrop) return;
	int nDrops = DragQueryFile(hd, 0xFFFFFFFF, buffer, MAX_PATH);

	if (nDrops > 0) {
		files.clear();
		printf("DragFiles count %d\n", nDrops);
		for (int i = 0; i < nDrops; i++) {
			DragQueryFile(hd, i, buffer, MAX_PATH);
			files.append(buffer).append("\n");
		}
		InvalidateRect(hwnd, NULL, FALSE);
	}
	DragFinish(hd);
}
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_PAINT: {
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		OnPaint(dc);
		EndPaint(hwnd, &ps);
	}break;
	case WM_DROPFILES: {
		OnDropFile(hwnd, wp);
	}break;
	case WM_SIZE:{
		GetClientRect(hwnd, &drop_rc);
		InflateRect(&drop_rc, -50, -80);
	}break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}