#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>

#define WC_WINDOW "WC_WINDOW"
#define NONCLIENT_THICKNESS 20
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
RECT ncRect[3];
//ncRect[0] : NonClient Rect	#Window Coord
//ncRect[1] : Draw Rect			#Region Coord
//ncRect[2] : Hittest Rect      #Client Coord
BOOL bHit[2] = { FALSE, FALSE };

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

	HWND hwnd = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW, WS_POPUP | WS_VISIBLE, 500, 200, 700, 300, nullptr, 0, wc.hInstance, NULL);
	ShowWindow(hwnd, TRUE);

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
	case WM_NCPAINT:
	{
		printf("WM_NCPAINT\n");
		//HRGN rgn = CreateRectRgnIndirect(&ncRect);
		if (wp == NULLREGION) {
			printf("NULLREGION\n");
		}
		HRGN rgn = nullptr;
		rgn = CreateRectRgn(ncRect[0].left, ncRect[0].top, ncRect[0].right, ncRect[0].bottom);

		HDC dc = GetDCEx(hwnd, rgn, DCX_WINDOW | DCX_CACHE | DCX_INTERSECTRGN | DCX_LOCKWINDOWUPDATE);
		HBRUSH br = CreateSolidBrush(RGB(145, 146, 147));
		HPEN pen = CreatePen(PS_INSIDEFRAME, 2, RGB(70, 180, 238));
		HGDIOBJ obj = SelectObject(dc, pen);
		HGDIOBJ oldbr = SelectObject(dc, br);

		Rectangle(dc, ncRect[1].left, ncRect[1].top, ncRect[1].right, ncRect[1].bottom);
		SetBkMode(dc, TRANSPARENT);
		SetTextColor(dc, RGB(220, 220, 220));
		DrawText(dc, "Non-Client Area", -1, &ncRect[1], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		SelectObject(dc, obj);
		ReleaseDC(hwnd, dc);
		DeleteObject(rgn);
		DeleteObject(br);
		DeleteObject(pen);
		return 0;
		break;
	}
	case WM_NCCALCSIZE:
	{
		printf("WM_NCCALSIZE\n");
		LPNCCALCSIZE_PARAMS lpParam = (LPNCCALCSIZE_PARAMS)lp;
		//copy entire source rect
		ncRect[0] = lpParam->rgrc[0];
		//calc non-client rect to make HRGN
		ncRect[0].bottom = ncRect[0].top + NONCLIENT_THICKNESS;
		//calc non-client rect to draw(to make coord 0,0,right,bottom)
		ncRect[1] = RECT{ 0,0,ncRect[0].right - ncRect[0].left, ncRect[0].bottom - ncRect[0].top };
		//calc NCHITTEST rect from HRGN to actually Client Coord
		ncRect[2] = ncRect[1];
		OffsetRect(&ncRect[2], 0, -NONCLIENT_THICKNESS);
		//calc Screen client rect 
		lpParam->rgrc[0].top += NONCLIENT_THICKNESS;
		return 0;
		break;
	}
	case WM_ACTIVATE:
	{
		printf("WM_ACTIVE\n");
		RedrawWindow(hwnd, NULL, NULL, RDW_UPDATENOW);
		return 0;
		break;
	}
	case WM_PAINT:
	{
		printf("WM_PAINT\n");
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		RECT rc;
		GetClientRect(hwnd, &rc);
		HPEN pen = CreatePen(PS_INSIDEFRAME, 2, RGB(56, 140, 220));
		SelectObject(dc, pen);
		SelectObject(dc, GetStockObject(DKGRAY_BRUSH));
		if (bHit[1])
			Rectangle(dc, rc.left, rc.top, rc.right, rc.bottom);
		SetBkMode(dc, TRANSPARENT);
		SetTextColor(dc, RGB(220, 220, 220));
		DrawText(dc, "Client Area", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		DeleteObject(pen);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_NCHITTEST:
	{
		LRESULT hit = HTCLIENT;
		POINT pos{ LOWORD(lp), HIWORD(lp) };
		ScreenToClient(hwnd, &pos);
		if (PtInRect(&ncRect[2], pos)) {
			printf("hit y: %d\n", pos.y);
			hit = HTCAPTION;
		}
		return hit;
		break;
	}
	case WM_LBUTTONDOWN:
	{
		POINT pos{ LOWORD(lp), HIWORD(lp) };
		printf("hmm");
		//RedrawWindow(hwnd, NULL, NULL, RDW_UPDATENOW);
		//RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
	

		//InvalidateRect(hwnd, NULL, TRUE);
		RedrawWindow(hwnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
		break;
	}


	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
