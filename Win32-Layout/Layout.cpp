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
	int	 magin = 0;
	int  fs    = 0;
};

Layout bbox;	//base layout
Layout hbox[2];	//hbox layout

int main(int args, char* argv[])
{
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = WC_MAINWINDOW;
	wc.lpfnWndProc = WndProc;
	assert(RegisterClassEx(&wc));
	wc.lpszClassName = WC_LAYOUT;
	wc.lpfnWndProc = LayoutProc;
	assert(RegisterClassEx(&wc));

	int w = 780;
	int h = 480;
	int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

	CreateWindowEx(NULL, WC_MAINWINDOW, "main window", WS_VISIBLE | WS_POPUPWINDOW,
		x, y, w, h, nullptr, (HMENU)0, wc.hInstance, 0);
	hbox[0];

	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.lParam;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	
	if (msg == WM_NCCREATE) {
		HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
		bbox.magin = 10;
		bbox.hwnd = CreateWindowEx(0, WC_LAYOUT, "layout", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0,
			hwnd, 0, hInst, &bbox);

		hbox[0].hwnd = CreateWindowEx(0, WC_LAYOUT, "hboxlayout0", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0,
			bbox.hwnd, 0, hInst, &hbox[0]);
		hbox[1].hwnd = CreateWindowEx(0, WC_LAYOUT, "hboxlayout1", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0,
			bbox.hwnd, 0, hInst, &hbox[1]);
	}
	if (msg == WM_SIZE) {
		SendMessage(bbox.hwnd, WM_SIZE, 0, 0);
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
		printf("wm_size %s\n", Wnd32::GetHwndText(hwnd));
		HWND parent = ::GetParent(hwnd);
		if (!parent) break;
		int parentChildNum = Wnd32::GetChildsNum(parent);
		if (parentChildNum == 1) {
			RECT prc;
			int magin = layout->magin;
			::GetClientRect(parent, &prc);
			InflateRect(&prc, -magin, -magin);
			Wnd32::SetGeometry(hwnd, prc);
		}
		int nChildNum = Wnd32::GetChildsNum(hwnd);
		if (!nChildNum) break;
		RECT rc;
		GetClientRect(hwnd, &rc);
		std::vector<HWND> childList(nChildNum);
		
		int w = rc.right - rc.left;
		int aw = w;
		int offset = 0;
		int ps = w / nChildNum;
		Wnd32::GetChildHwndList(hwnd, nChildNum, childList.data());
		for (int i = 0; i < nChildNum; ++i) {
			HWND child = childList[i];
			printf("child %s\n", Wnd32::GetHwndText(child));
			RECT r = rc;
			r.left = r.left + offset;
			r.right = r.left + ps;
			offset += ps;
			Wnd32::SetGeometry(child, r);
		}
	}break;
	case WM_PAINT: {
		printf("wm_paint %s\n", Wnd32::GetHwndText(hwnd));
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