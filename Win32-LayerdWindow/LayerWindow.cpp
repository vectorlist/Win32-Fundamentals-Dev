#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

//Create a GDI + Bitmap object with the PixelFormat32bppPARGB pixel format.
//Create a Graphics object to draw in this Bitmap object.
//Do all your drawing into this object using GDI + .
//Destroy the Graphics object created in step 2.
//Call the GetHBITMAP method on the Bitmap object to get a Windows HBITMAP.
//Destroy the Bitmap object.
//Create a memory DC using CreateCompatibleDC and select the HBITMAP from step 5 into it.
//Call UpdateLayeredWindow using the memory DC as a source.
//Select previous bitmap and delete the memory DC.
//Destroy the HBITMAP created in step 5.

#include <stdio.h>
#include <iostream>
#include <ObjIdl.h>
#include <gdiplus.h>
#include <gdiplusheaders.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_WIDTH 800
#define MAX_HEIGHT 600

using namespace std;

void Drawtext(HWND hwnd, HDC hdc);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_DESTROY) {

		PostQuitMessage(0);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
};

HINSTANCE hinst;

int  main(int args, char* argv[]) {
	HWND hWnd;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	hinst = GetModuleHandle(NULL);

	//Initialize GDI+
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	hinst = GetModuleHandle(NULL);
	// create a window class:
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hinst;
	wc.lpszClassName = "win32";

	// register class with operating system:
	RegisterClass(&wc);

	// create and show window:
	hWnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST,
		"win32",
		"WinSoup",
		WS_POPUP,
		0, 0, 1000, 500,
		nullptr,
		nullptr,
		hinst,
		nullptr
	);

	if (hWnd == NULL) {
		return 0;
	}

	Drawtext(hWnd, GetDC(hWnd));

	ShowWindow(hWnd, SW_SHOW);


	MSG msg = {};

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}

void Drawtext(HWND hwnd, HDC hdc)
{
	FontFamily  fontFamily(L"Segoe UI");
	Font        font(&fontFamily, 54, FontStyleRegular, UnitPixel);
	PointF      pointF(30.0f, 10.0f);
	SolidBrush  solidBrush(Color(255, 70, 140, 200));

	Bitmap softwareBitmap(MAX_WIDTH, MAX_HEIGHT, PixelFormat32bppARGB);
	Graphics g(&softwareBitmap);

	g.Clear(Gdiplus::Color(60, 0, 0, 0));  // 30: alpha value 

	g.DrawString(L"This is Not Hardcore", -1, &font, pointF, &solidBrush);
	

	HBITMAP bmp;
	softwareBitmap.GetHBITMAP(Color(0, 0, 0, 0), &bmp);

	HDC memdc = CreateCompatibleDC(hdc);
	HGDIOBJ original = SelectObject(memdc, bmp);


	BLENDFUNCTION blend = { 0 };
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;
	blend.AlphaFormat = AC_SRC_ALPHA;
	POINT ptLocation = { 500, 300 };
	SIZE szWnd = { MAX_WIDTH, MAX_HEIGHT };
	POINT ptSrc = { 0, 0 };
	BOOL l = UpdateLayeredWindow(hwnd, hdc, &ptLocation, &szWnd,
		memdc, &ptSrc, 0, &blend, ULW_ALPHA);
	int err = GetLastError();
	SelectObject(hdc, original);

	DeleteObject(bmp);
	DeleteObject(memdc);
	
}