#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_MARNINGS
#include <Windows.h>
#include <iostream>
#include <ObjIdl.h>
#include <gdiplus.h>
#include <gdiplusheaders.h>
#pragma comment(lib,"gdiplus.lib")
#include <../Common/Log.h>
#include <../Common/Wnd32.h>

#define WIDTH 380
#define HEIGHT 320
using  namespace Gdiplus;

#define UWM_LAYERED_PAINT (WM_APP + 1) 

struct LAYEREDPAINTSTRUCT
{
	HWND	hwnd;
	RECT	localRc;
	HDC		dc;
	DWORD	id;
};

HINSTANCE hInst;
HWND wnd[3];
Image* image[2];
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

int main(int args, char* argv[]) {
	GdiplusStartupInput input;
	ULONG_PTR tokken;
	GdiplusStartup(&tokken, &input, NULL);

	hInst = GetModuleHandle(NULL);

	WNDCLASSEX wc{};
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.hbrBackground = CreateSolidBrush(RGB(45, 45, 45));
	wc.lpfnWndProc   = WndProc;
	wc.hInstance     = hInst;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "Wnd32";
	wc.style         = CS_VREDRAW | CS_HREDRAW;

	RegisterClassEx(&wc);

	image[0] = Image::FromFile(L"../Data/layer.png");
	image[1] = Image::FromFile(L"../Data/layer2.png");

	wnd[0] = CreateWindowEx(WS_EX_LAYERED 
		, wc.lpszClassName, wc.lpszClassName, WS_VISIBLE | WS_POPUP,
		600, 200, image[0]->GetWidth(), image[0]->GetHeight(), NULL, 0, hInst, 0);

	wnd[1] = CreateWindowEx(
		NULL,
		wc.lpszClassName, "Close", WS_VISIBLE | WS_CHILD,
		440, 300, 120,50, wnd[0], (HMENU)1, hInst, 0);
	wnd[2] = CreateWindowEx(
		NULL,
		wc.lpszClassName, "Layered Draw", WS_VISIBLE | WS_CHILD,
		280, 300, 120, 50, wnd[0], (HMENU)2, hInst, 0);


	LOG::LogLastError();

	printf("show window\n");
	ShowWindow(wnd[0], SW_SHOW);
	InvalidateRect(wnd[0], NULL, FALSE);

	MSG msg{};
	while (GetMessage(&msg, NULL, 0,0))
	{
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
	return (int)msg.lParam;
}

void MainLayeredPaint(HWND hwnd, HDC dc) {
	printf("drawing image\n");
	Image* img = image[0];
	SIZE sz{ img->GetWidth(), img->GetHeight() };
	RECT rc;
	POINT pt{ 0 };
	GetWindowRect(hwnd, &rc);

	HDC memDc = CreateCompatibleDC(dc);
	HBITMAP bmp = CreateCompatibleBitmap(dc, img->GetWidth(), img->GetHeight());

	HBITMAP oriBmp = (HBITMAP)SelectObject(memDc, bmp);

	Graphics g(memDc);

	FontFamily  fontFamily(L"Segoe UI");
	Font        font(&fontFamily, 22, FontStyleRegular, UnitPixel);
	PointF      pointF(100.0f, 110.0f);
	SolidBrush  solidBrush(Color(255, 220, 220, 220));

	g.DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight());
	g.DrawString(L"Win32 Layered Window", -1, &font, pointF, &solidBrush);

	
	const UINT nChilds = Wnd32::GetChildsNum(hwnd);
	
	std::vector<HWND> pChilds(nChilds);
	Wnd32::GetChildHwndList(hwnd, nChilds, pChilds.data());
	for (auto child : pChilds) {
		LAYEREDPAINTSTRUCT lps{};
		lps.hwnd = child;
		lps.dc = memDc;
		lps.localRc = Wnd32::GetLocalCoordRect(child);
		lps.id = GetDlgCtrlID(child);
		SendMessage(child, UWM_LAYERED_PAINT, 0, (LONG_PTR)&lps);
	}

	BLENDFUNCTION blend = { 0 };
	blend.BlendOp             = AC_SRC_OVER;
	blend.AlphaFormat         = AC_SRC_ALPHA;
	blend.SourceConstantAlpha = 255;
	BOOL bRet = UpdateLayeredWindow(hwnd, dc, ((LPPOINT)&rc),
		&sz, memDc, &pt, 0x00000000, &blend, ULW_ALPHA);

	SelectObject(memDc, oriBmp);
	DeleteDC(memDc);
	DeleteObject(bmp);
}

void OnLayeredPaint(LAYEREDPAINTSTRUCT* lps) {
	Graphics g(lps->dc);
	RECT rc = lps->localRc;
	g.DrawImage(image[1], rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);

	FontFamily  fontFamily(L"Segoe UI");
	Font        font(&fontFamily, 14, FontStyleRegular, UnitPixel);
	RectF        rect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	StringFormat fm;
	fm.SetAlignment(StringAlignment::StringAlignmentCenter);
	fm.SetLineAlignment(StringAlignment::StringAlignmentCenter);
	SolidBrush  br(Color(255, 220, 220, 220));
	g.DrawString(Wnd32::StrToWideStr(Wnd32::GetHwndText(lps->hwnd)), -1,
		&font, rect, &fm, &br);
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_PAINT: {
		if (hwnd != wnd[0])
			return TRUE;
		printf("WM_PAINT %s\n", Wnd32::GetHwndText(hwnd));
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		MainLayeredPaint(hwnd, dc);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_LBUTTONDOWN: {
		printf("WM_LBUTTONDOWN %s\n", Wnd32::GetHwndText(hwnd));
		if (wnd[1] == hwnd) {
			PostQuitMessage(0);
		}
		break;
	}
	case UWM_LAYERED_PAINT:{
		printf("UWM_LAYERED_PAINT %s\n", Wnd32::GetHwndText(hwnd));
		OnLayeredPaint((LAYEREDPAINTSTRUCT*)lp);
		break;
	}
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}