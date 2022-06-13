#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdint.h>
#include <iostream>

#define WIDTH	860
#define HEIGHT	480
typedef uint32_t uint32;
HWND window;


struct RGBA
{
	RGBA(){}
	RGBA(int r, int g, int b, int a) :r(r), g(g), b(b), a(a){}
	BYTE r, g, b, a;
};

struct Pixel {
	Pixel(int w, int y) : width(w), height(y){
		data = new RGBA[w * y];
	}
	Pixel() {
		delete data;
	}

	bool ClearColor(const RGBA& col) {
		RGBA* p = (RGBA*)data;
		for (int i = 0; i < width * height; ++i) {
			BYTE* b = (BYTE*)p;
			b[0] = col.b;
			b[1] = col.g;
			b[2] = col.r;
			b[3] = col.a;
			p++;
		}
		return true;
	}
	int width;
	int height;
	RGBA* data;
};

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
Pixel* pixel;
HBITMAP map;
int frame = 0;

int main(int args, char* argv[])
{
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(wc);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = "PIXEL WINDOW";
	wc.lpfnWndProc = WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	auto atom = RegisterClassEx(&wc);

	

	int x = (GetSystemMetrics(SM_CXSCREEN) - WIDTH)/ 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - HEIGHT)/ 2;
	window = CreateWindowEx(NULL, wc.lpszClassName, wc.lpszClassName, WS_VISIBLE | WS_POPUP,
		x, y, WIDTH, HEIGHT, nullptr, (HMENU)0, wc.hInstance, 0);

	ShowWindow(window, SW_SHOW);

	pixel = new Pixel(WIDTH, HEIGHT);

	BITMAPINFO info{};
	info.bmiHeader.biSize = sizeof(info.bmiHeader);
	info.bmiHeader.biWidth = WIDTH;
	info.bmiHeader.biHeight = HEIGHT;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 32;
	info.bmiHeader.biCompression = BI_RGB;

	HDC dc = GetDC(window);
	
	MSG msg{};
	int b = 0;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
		pixel->ClearColor(RGBA(frame,frame,frame,0));
		StretchDIBits(dc, 0, 0, pixel->width, pixel->height,
			0, 0, pixel->width, pixel->height,
			pixel->data, &info, DIB_RGB_COLORS, SRCCOPY);
		RECT rc;
		GetClientRect(window, &rc);
		SetTextColor(dc, RGB(220, 220, 220));
		SetBkMode(dc, TRANSPARENT);
		DrawText(dc, "Click me here", -1, &rc, DT_SINGLELINE | DT_CENTER |DT_VCENTER);
	}
	delete pixel;
	return msg.lParam;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	static bool b = false;
	static bool rev = false;
	switch (msg)
	{
	case WM_LBUTTONDOWN: {
		if (!b) {
			SetTimer(hwnd, 0, 33, 0);
			b = true;
		}
		else {
			KillTimer(hwnd, 0);
			b = false;
		}
		break;
	}
	case WM_TIMER: {
		printf("%d\n",frame);
		if (frame > 80) {
			rev = true;
		}
		if (frame <= 22)
			rev = false;
		if (rev)
			frame--;
		else
			frame++;
		break;
	}
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}