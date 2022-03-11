#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <assert.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>

#pragma comment(lib,"D2D1")
#pragma comment(lib,"DWrite")

struct D2DWindow
{
	ID2D1HwndRenderTarget *pRenterTaget;
	LPCSTR name;
	HWND hwnd;
};

struct Graphics
{
	ID2D1Factory* factory;
	IDWriteFactory* writeFactory;
	IDWriteTextFormat* textFormat;
};

Graphics graphics;
D2DWindow window;
ID2D1SolidColorBrush* brush;
ID2D1SolidColorBrush* txtBrush;


#define WC_WINDOW "window"
HWND hWnd;
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

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

	HRESULT hr;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &graphics.factory);

	if (SUCCEEDED(hr)) {
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(graphics.writeFactory),
			(IUnknown**)(&graphics.writeFactory));
		
	}

	window.hwnd = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW, WS_POPUP | WS_VISIBLE,
		500, 200, 700, 300, nullptr, 0, wc.hInstance, &window);

	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.lParam;
}


void CreateRenderTarget(D2DWindow* window) {
	if (window->pRenterTaget) return;
	RECT rc;
	GetClientRect(window->hwnd, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
	HRESULT hr = graphics.factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(window->hwnd, size),
		&window->pRenterTaget);

	if (SUCCEEDED(hr)) {
		const D2D1_COLOR_F color = D2D1::ColorF(0.1f, 0.1f, 0.1f);
		hr = window->pRenterTaget->CreateSolidColorBrush(
			color, &brush);
	}
	if (SUCCEEDED(hr)) {
		const D2D1_COLOR_F color = D2D1::ColorF(0.98, 0.98, 0.98);
		hr = window->pRenterTaget->CreateSolidColorBrush(
			color, &txtBrush);
	}
	if (SUCCEEDED(hr)) {
		hr = graphics.writeFactory->CreateTextFormat(
			L"Segoe UI  Symbol",
			NULL,
			DWRITE_FONT_WEIGHT_THIN,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			14,
			L"en-us",
			&graphics.textFormat);
	}
}
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	D2DWindow* window = (D2DWindow*)GetWindowLong(hwnd, GWL_USERDATA);
	switch (msg)
	{
	case WM_NCCREATE: {
		window = (D2DWindow*)((LPCREATESTRUCT)lp)->lpCreateParams;
		assert(window);
		window->hwnd = hwnd;
		SetWindowLong(hwnd, GWL_USERDATA, (LONG)window);
		break;
	}
	case WM_CREATE:
	{
		//OnCreate(window);
		break;
	}
	case WM_SIZE:
	{

		//OnSize(window, lp);
		break;
	}
	case WM_PAINT:
	{
		CreateRenderTarget(window);
		auto renderTarget = window->pRenterTaget;
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		renderTarget->BeginDraw();
		renderTarget->Clear(D2D1::ColorF(0.2, 0.2, 0.2));

		auto  rc  = D2D1::RectF(100,100,230,50);
		
		renderTarget->FillRectangle(rc, brush);
		TCHAR code[125] = "Direct2D";
		std::wstring wcode(code, code + sizeof(code));
		renderTarget->DrawText(wcode.c_str(), wcode.length(), graphics.textFormat, rc, txtBrush);
		
		//renderTarget->DrawRectangle()
		renderTarget->EndDraw();
		EndPaint(hwnd, &ps);
		break;
	}
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}