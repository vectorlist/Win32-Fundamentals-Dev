#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Uxtheme.h>
#pragma comment(lib, "UxTheme.lib")
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

#define WC_WINDOW	"WC_WINDOW"
#define WC_CHILD	"WC_CHILD"
#define WC_TOOLTIP	"WC_TOOLTIP"
#include <string>
struct Window
{
	HWND hwnd;
	LPCSTR name;
	HBRUSH br = nullptr;
};

Window mainWindow;
Window child;
Window child2;
Window tooltip;
HINSTANCE hInst;

void	SetToolTip(HWND srcHwnd, HWND tHwnd, LPCSTR text);
LPCSTR	GetToolTipText(HWND srcHwnd, HWND tHwnd);

int main(int args, char argv[]) {

	InitCommonControls();

	hInst = GetModuleHandle(NULL);

	WNDCLASSEX wc{};
	wc.cbSize = sizeof(wc);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hInstance = hInst;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = WC_WINDOW;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WndProc;
	GetClassInfoEx(wc.hInstance, wc.lpszClassName, &wc);
	RegisterClassEx(&wc);

	//SuperClass Clone ToolTip
	WNDCLASSEX ttwc = {};
	ttwc.cbSize = sizeof(wc);
	ttwc.lpszClassName = TOOLTIPS_CLASS;
	GetClassInfoEx(GetModuleHandle(NULL), ttwc.lpszClassName, &ttwc);
	//Remove defualt shadow style
	if (ttwc.style & CS_DROPSHADOW) {
		ttwc.style &= ~CS_DROPSHADOW;
	}
	ttwc.lpszClassName = WC_TOOLTIP;

	if (RegisterClassEx(&ttwc)) {
		printf("Registered %s class\n", ttwc.lpszClassName);
	}
	LPCSTR co = ttwc.lpszClassName;
	std::wstring code(co, co + sizeof(co));

	mainWindow.name = "main window";
	mainWindow.br = CreateSolidBrush(RGB(45, 45, 45));
	mainWindow.hwnd = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW, WS_POPUP | WS_VISIBLE,
		500, 200, 700, 300, nullptr, 0, wc.hInstance, &mainWindow);

	wc.lpszClassName = WC_CHILD;
	RegisterClassEx(&wc);
	child.name = "first window";
	child.br = CreateSolidBrush(RGB(88, 88, 88));
	child.hwnd = CreateWindowEx(NULL, wc.lpszClassName, child.name, WS_CHILD | WS_VISIBLE,
		130, 70, 200, 100, mainWindow.hwnd, 0, wc.hInstance, &child);

	child2.name = "second window";
	child2.br = CreateSolidBrush(RGB(100, 100, 88));
	child2.hwnd = CreateWindowEx(NULL, wc.lpszClassName, child.name, WS_CHILD | WS_VISIBLE,
		360, 70, 200, 100, mainWindow.hwnd, 0, wc.hInstance, &child2);

	tooltip.name = WC_TOOLTIP;
	//tooltip.br = CreateSolidBrush(RGB(40, 80, 180));
	tooltip.br = CreateSolidBrush(RGB(22, 22, 22));
	tooltip.hwnd = CreateWindowEx(NULL, ttwc.lpszClassName, 0, WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		child.hwnd, 0, hInst, 0);

	SetToolTip(child.hwnd, tooltip.hwnd, "this is first child\r cool window");
	SetToolTip(child2.hwnd, tooltip.hwnd, "this is second child");

	SetWindowTheme(tooltip.hwnd, L"", L"");

	ShowWindow(mainWindow.hwnd, TRUE);

	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
	}
	return (int)msg.lParam;
}
//TODO : clac multiline text size
SIZE GetMultiLineSize(HWND hwnd, LPCSTR text, UINT lineWidth) {
	SIZE size{0};
	HDC dc = GetDC(hwnd);
	HFONT font = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
	SelectObject(dc, font);
	std::string str(text);
	GetTextExtentPoint32(dc, text, strlen(text), &size);
	int line_x = size.cx;
	int line_num = 1;
	int temp_line_x = min(size.cx, lineWidth);

	while (line_x >= lineWidth)
	{
		line_num++;
		line_x -= lineWidth;
		temp_line_x = max(temp_line_x, line_x);
	} 
	size.cx = temp_line_x;
	size.cy = size.cy * line_num;
	return size;
}

void SetToolTip(HWND srcHwnd, HWND tHwnd, LPCSTR text) {
	TOOLINFO info{};
	info.cbSize = sizeof(TOOLINFO);
	info.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
	info.hwnd = srcHwnd;
	info.uId = (UINT_PTR)srcHwnd;
	info.lpszText = (LPSTR)text;
	GetClientRect(srcHwnd, &info.rect);
	SendMessage(tHwnd, TTM_ADDTOOL, 0, (LPARAM)&info);
}

LPCSTR GetToolTipText(HWND srcHwnd, HWND tHwnd) {
	static std::basic_string<TCHAR> tempStr;
	tempStr.clear();
	tempStr.reserve(128);

	TOOLINFO info{};
	info.cbSize = sizeof(info);
	info.hwnd = srcHwnd;
	info.uId = (UINT_PTR)srcHwnd;
	info.lpszText = (LPSTR)tempStr.data();
	SendMessage(tHwnd, TTM_GETTEXT, 0, (LPARAM)&info);
	return info.lpszText;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (msg)
	{
	case WM_NCCREATE: {
		window = (Window*)((LPCREATESTRUCT)lp)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
		break;
	}
	case WM_NOTIFY: {
		NMHDR* hdr = (LPNMHDR)lp;
		if (hdr->hwndFrom == tooltip.hwnd) {
			switch (hdr->code)
			{
			case NM_CUSTOMDRAW: {
				printf("Notify NM_CUSTOMDRAW\n");
				NMTTCUSTOMDRAW* tcd = (NMTTCUSTOMDRAW*)hdr;
				HDC dc = tcd->nmcd.hdc;
				RECT rc = tcd->nmcd.rc;
				UINT stage = tcd->nmcd.dwDrawStage;
				switch (stage)
				{
				case CDDS_PREPAINT: {
					printf("Notify CDDS_PREPAINT\n");
					RECT fixedRc = rc;
					SelectObject(dc, tooltip.br);
					SelectObject(dc, (HPEN)GetStockObject(DC_PEN));
					SetDCPenColor(dc, RGB(133, 133, 133));
					Rectangle(dc, rc.left, rc.top, rc.right, rc.bottom);
					LPCSTR text = GetToolTipText(hwnd, hdr->hwndFrom);
					std::string t(text);
					SetBkMode(dc, TRANSPARENT);
					SetTextColor(dc, RGB(220, 220, 220));
					DrawText(dc, text, t.size(), &fixedRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					return CDRF_SKIPDEFAULT;
				}
				}
				return CDRF_DODEFAULT;
				break;
			}
			case TTN_SHOW: {
				printf("Notify TTN_SHOW\n");
				LPCSTR text = GetToolTipText(hwnd, hdr->hwndFrom);
				HDC dc = GetDC(hdr->hwndFrom);
				HFONT font = (HFONT)SendMessage(hdr->hwndFrom, WM_GETFONT, 0, 0);
				SelectObject(dc, font);
				SIZE textSize{0};
				GetTextExtentPoint32(dc, text, strlen(text), &textSize);
				//SIZE textSize = GetMultiLineSize(tooltip.hwnd, text, 60);
				RECT rc;
				GetWindowRect(hdr->hwndFrom, &rc);
				rc.right = rc.left + textSize.cx;
				rc.bottom = rc.top + textSize.cy;

				int w = rc.right - rc.left + 16;
				int h = rc.bottom - rc.top  + 12;

				SendMessage(hdr->hwndFrom, TTM_ADJUSTRECT, TRUE, (LPARAM)&rc);
				SetWindowPos(hdr->hwndFrom, NULL, rc.left, rc.top, w, h,
					SWP_NOZORDER | SWP_NOACTIVATE);
				return TRUE;	
			}
			}
		}
		break;
	}
	case WM_PAINT:{
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		RECT rc;
		GetClientRect(hwnd, &rc);
		FillRect(dc, &rc, window->br);
		SetBkMode(dc, TRANSPARENT);
		SetTextColor(dc, RGB(220, 220, 220));
		DrawText(dc, window->name, -1, &rc, DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &ps);
		break;
	}
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
