#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <assert.h>
#include <../Common/Wnd32.h>

#define WC_MAINWINDOW	"MAINWINDOW"
#define WC_LAYOUT		"LAYOUT"

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT WINAPI LayoutProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

struct Layout
{
	HWND hwnd  = nullptr;
	int	 magin = 20;
	int  fs    = 0;
	bool aligmentVert = false;
};

Layout bbox;	//base layout
Layout hbox[3];	//hbox layout
Layout vbox[1];	//vbox layout

int main(int args, char* argv[])
{
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = WC_MAINWINDOW;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WndProc;
	assert(RegisterClassEx(&wc));
	wc.lpszClassName = WC_LAYOUT;
	wc.lpfnWndProc = LayoutProc;
	assert(RegisterClassEx(&wc));

	int w = 860;
	int h = 480;
	int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

	CreateWindowEx(NULL, WC_MAINWINDOW, "main window",
		WS_VISIBLE | WS_POPUP | WS_SIZEBOX,
		x, y, w, h, nullptr, (HMENU)0, wc.hInstance, 0);

	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.lParam;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	
	switch (msg)
	{
	case WM_NCCREATE:{
		HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
		bbox.magin = 10;
		bbox.hwnd = CreateWindowEx(0, WC_LAYOUT, "layout", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0,
			hwnd, 0, hInst, &bbox);

		hbox[0].hwnd = CreateWindowEx(0, WC_LAYOUT, "hboxlayout0", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0,
			bbox.hwnd, 0, hInst, &hbox[0]);

		vbox[0].aligmentVert = true;
		vbox[0].hwnd = CreateWindowEx(0, WC_LAYOUT, "vboxlayout0", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0,
			bbox.hwnd, 0, hInst, &vbox[0]);

		hbox[2].hwnd = CreateWindowEx(0, WC_LAYOUT, "hboxlayout1", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0,
			vbox[0].hwnd, 0, hInst, &hbox[2]);

		hbox[3].magin = 20;
		hbox[3].hwnd = CreateWindowEx(0, WC_LAYOUT, "hboxlayout2", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0,
			vbox[0].hwnd, 0, hInst, &hbox[3]);

	}break;
	case WM_SIZE: {
		SendMessage(bbox.hwnd, WM_SIZE, 0, 0);
	}break;
	case WM_KEYDOWN: {
		if (wp == VK_ESCAPE) {
			DestroyWindow(hwnd);
		}
	}break;
	case WM_NCDESTROY:{
		PostQuitMessage(0);
	}break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT WINAPI LayoutProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	Layout* layout = (Layout*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (msg)
	{
	case WM_NCCREATE: {
		layout = (Layout*)((LPCREATESTRUCT)lp)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)layout);
	}break;
	case WM_SIZE: {
		//printf("wm_size %s\n", Wnd32::GetHwndText(hwnd));
		HWND parent = ::GetParent(hwnd);
		int parentChildNum = 0;
		if (parent) {
			parentChildNum = Wnd32::GetChildsNum(parent);
		}
		if (parent && (parentChildNum == 1)) {
			RECT prc;
			int magin = layout->magin;
			::GetClientRect(parent, &prc);
			InflateRect(&prc, -magin, -magin);
			Wnd32::SetGeometry(hwnd, prc);
		}
		int nChild = Wnd32::GetChildsNum(hwnd);
		if (!nChild) break;
		RECT rc;
		GetClientRect(hwnd, &rc);
		
		bool bVert = layout->aligmentVert;

		int w = rc.right - rc.left;
		if (bVert) {
			w = rc.bottom - rc.top;
		}
		int offset = 0;
		int ps = w / nChild;

		std::vector<HWND> childs(nChild);

		Wnd32::GetChildHwndList(hwnd, nChild, childs.data());

		HDWP hdwp = BeginDeferWindowPos(nChild);
		for (int i = 0; i < nChild; ++i) {
			HWND child = childs[i];
			RECT r = rc;
			if (bVert) {
				r.top = r.top + offset;
				r.bottom = r.top + ps;
			}
			else {
				r.left = r.left + offset;
				r.right = r.left + ps;
			}
			offset += ps;
			Layout* l = (Layout*)GetWindowLongPtr(child, GWLP_USERDATA);
			int margin = l->magin;
			InflateRect(&r, -margin, -margin);
			
			::DeferWindowPos(hdwp, child, HWND_BOTTOM, r.left, r.top,
				r.right - r.left, r.bottom - r.top, NULL);
			SendMessage(child, WM_SIZE, 0, 0);
		}
		EndDeferWindowPos(hdwp);
	}break;
	case WM_PAINT: {
		printf("WM_PAINT %s\n", Wnd32::GetHwndText(hwnd));
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		Pen pen(1, RGB(120, 120, 120));
		Brush br(RGB(42, 42, 42));
		SelectObject(dc, pen);
		SelectObject(dc, br);
		Wnd32::DrawRoundRect(dc, ps.rcPaint, 8, 8);
		SetTextColor(dc, RGB(200, 200, 200));
		SetBkMode(dc, TRANSPARENT);
		DrawText(dc, Wnd32::GetHwndText(hwnd), -1,&ps.rcPaint,DT_CENTER | DT_SINGLELINE);
		EndPaint(hwnd, &ps);
	}break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}