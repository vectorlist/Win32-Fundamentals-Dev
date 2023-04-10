#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <assert.h>
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <../Common/Wnd32.h>

#define WC_MAIN		"WC_MAIN"
#define WIDTH		840
#define HEIGHT		460

HWND main_hwnd = nullptr;

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

int main(int args, char* argv[])
{
	InitCommonControls();
	WNDCLASSEX wc{};
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_VREDRAW | CS_HREDRAW;
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hInstance     = GetModuleHandle(0);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.lpfnWndProc   = WndProc;
	wc.lpszClassName = WC_MAIN;

	ATOM mainAtom = RegisterClassExA(&wc);


	int x = (GetSystemMetrics(SM_CXSCREEN) - WIDTH) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - HEIGHT) / 2;

	main_hwnd = CreateWindowEx(NULL, WC_MAIN, WC_MAIN, WS_VISIBLE | WS_POPUP,
		x, y, WIDTH, HEIGHT, NULL, (HMENU)0, wc.hInstance, 0);

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
	static HWND buttons[3];
	static WINDOWPLACEMENT g_wp{};
	static HFONT font = nullptr;
	switch (msg)
	{
	case WM_CREATE: {
		HINSTANCE hInst = (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE);

		buttons[0] = CreateWindowEx(NULL, WC_BUTTON, "full screen",
			WS_VISIBLE | WS_CHILD, 230, 50, 80, 20, hwnd,
			(HMENU)0, hInst, NULL);
		buttons[1] = CreateWindowEx(NULL, WC_BUTTON, "restore",
			WS_VISIBLE | WS_CHILD, 330, 50, 80, 20, hwnd,
			(HMENU)1, hInst, NULL);
		buttons[2] = CreateWindowEx(NULL, WC_BUTTON, "minimalize",
			WS_VISIBLE | WS_CHILD, 430, 50, 80, 20, hwnd,
			(HMENU)2, hInst, NULL);
		buttons[2] = CreateWindowEx(NULL, WC_BUTTON, "&close",
			WS_VISIBLE | WS_CHILD, 530, 50, 80, 20, hwnd,
			(HMENU)3, hInst, NULL);
		font = CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Segoe UI");
	}break;
	case WM_COMMAND: {
		switch (wp)
		{
		case 0: {
			printf("com1\n");
			if (!IsZoomed(hwnd)) {
				printf("not fulled now\n");
				SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
				SetFocus(hwnd);
			}
		}break;
		case 1: {
			if (IsZoomed(hwnd)) {
				SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
			}
		}break;
		case 2: {
			SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		}break;
		case 3: {
			SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
		}break;
		default:
			break;
		}
	}break;
	case WM_NOTIFY: {
		LPNMHDR hdr = (LPNMHDR)lp;
		HWND item = hdr->hwndFrom;
		if (item) {
			LPNMCUSTOMDRAW cd = (LPNMCUSTOMDRAW)lp;
			HDC dc = cd->hdc;
			HBRUSH br = (HBRUSH)GetStockObject(DC_BRUSH);
			SelectObject(cd->hdc, GetStockObject(DC_BRUSH));
			SelectObject(cd->hdc, font);
			if(cd->uItemState == CDIS_HOT)
				SetDCBrushColor(dc, RGB(99, 99, 99));
			else
				SetDCBrushColor(dc, RGB(88, 88, 88));
			FillRect(dc, &cd->rc, br);
			SetTextColor(dc, RGB(200, 200, 200));
			SetBkMode(dc, TRANSPARENT);
			CHAR buf[256];
			GetWindowText(item, buf, 256);
			DrawText(dc, buf, -1, &cd->rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			return CDRF_SKIPDEFAULT;
		}
		
	}break;
	case WM_KEYDOWN: {
		if (wp == VK_ESCAPE) SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		printf("%d\n",wp);
	}break;
	case WM_NCDESTROY:
		DeleteObject(font);
		PostQuitMessage(0);
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
