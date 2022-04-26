#include "Graphics.h"

Graphics::Graphics()
{
}

void Graphics::Create(HWND hwnd)
{
	mHwnd = hwnd;
	GetClientRect(hwnd, &mRect);
	mFrontDc = GetDC(hwnd);
	mBackDc = CreateCompatibleDC(mFrontDc);
	mBmpDc = CreateCompatibleDC(mFrontDc);
	
	mFrontBmp = CreateCompatibleBitmap(mBackDc, mRect.right, mRect.bottom);

	mOldFrontBmp = (HBITMAP)SelectObject(mBackDc, mFrontBmp);
	mOldDispBmp = (HBITMAP)SelectObject(mBackDc, mFrontBmp);
}

HBITMAP Graphics::LoadBmpFromFile(LPCSTR filename)
{
	return (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

void Graphics::RenderBitmap(HBITMAP bmp, int x, int y)
{
	BITMAP info{};
	GetObject(bmp, sizeof(BITMAP), &info);
	SelectObject(mBmpDc, bmp);
	BitBlt(mBackDc, x, y, info.bmWidth, info.bmHeight, mBmpDc, 0, 0, SRCCOPY);
}
