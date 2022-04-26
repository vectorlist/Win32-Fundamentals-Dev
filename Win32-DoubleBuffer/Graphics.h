#pragma once
#include <Windows.h>
#pragma comment(lib, "msimg32.lib")

class Graphics
{
public:
	Graphics();
	void Create(HWND hwnd);
	HBITMAP LoadBmpFromFile(LPCSTR filename);
	void RenderBitmap(HBITMAP bmp, int x, int y);
	HWND mHwnd;
	HDC mFrontDc;
	HDC mBackDc;
	HDC mBmpDc;

	HBITMAP mFrontBmp;
	HBITMAP mOldFrontBmp;
	HBITMAP mOldDispBmp;

	RECT mRect;

};

