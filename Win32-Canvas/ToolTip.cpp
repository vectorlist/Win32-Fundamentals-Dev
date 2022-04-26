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

#include <../Common/Wnd32.h>

#define WC_WINDOW	"WC_WINDOW"
#define WC_CHILD	"WC_CHILD"
#define WC_TOOLTIP	"WC_TOOLTIP"
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
WNDPROC PreWndProc = nullptr;
struct Window
{
	HWND hwnd;
	LPCSTR name;
	HBRUSH br = nullptr;
};

Window window;
Window child;
Window tooltip;

void DrawFillRect(HDC dc, const RECT &rc, COLORREF clr) {
	::SetBkColor(dc, clr);
	::ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
}

int main(int args, char* argv[])
{
	InitCommonControls();

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

	//SuperClass ToolTip
	WNDCLASSEX ttwc = {};
	ttwc.cbSize = sizeof(wc);
	ttwc.lpszClassName = TOOLTIPS_CLASS;
	GetClassInfoEx(GetModuleHandle(NULL), ttwc.lpszClassName, &ttwc);
	//Remove defualt shadow style
	if (ttwc.style & CS_DROPSHADOW) {
		ttwc.style &= ~CS_DROPSHADOW;
	}
	ttwc.lpszClassName = WC_TOOLTIP;
	PreWndProc = ttwc.lpfnWndProc;
	
	if (RegisterClassEx(&ttwc)) {
		printf("Registered %s class\n", ttwc.lpszClassName);
	}

	window.name = "main window";
	window.br = CreateSolidBrush(RGB(45, 45, 45));
	window.hwnd = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW, WS_POPUP | WS_VISIBLE, 
		500, 200, 700, 300, nullptr, 0, wc.hInstance, &window);

	wc.lpszClassName = WC_CHILD;
	RegisterClassEx(&wc);
	child.name = "child window";
	child.br = CreateSolidBrush(RGB(88, 88, 88));
	child.hwnd = CreateWindowEx(NULL, wc.lpszClassName, child.name, WS_CHILD | WS_VISIBLE,
		120, 70, 300, 100, window.hwnd, 0, wc.hInstance, &child);

	ShowWindow(window.hwnd, TRUE);
	
	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.lParam;
}

TOOLINFO GetToolInfo(HWND hwnd, UINT id) {
	TOOLINFO info{};
	info.cbSize = sizeof(TOOLINFO);
	info.hwnd = hwnd;
	info.uId = id;
	SendMessage(hwnd, TTM_GETTOOLINFO, 0, (LONG)&info);
	return info;
}

LRESULT TooTipCustomDraw(NMTTCUSTOMDRAW* lptCd) {
	
	LPNMCUSTOMDRAW lpCd = &lptCd->nmcd;
	switch (lpCd->dwDrawStage)
	{
	case CDDS_PREPAINT: {
		printf("CDDS_PREPAINT\n");
		RECT rc = lpCd->rc;
		HDC dc = lpCd->hdc;
		
		FillRect(dc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
		SetBkMode(dc, TRANSPARENT);
		SetTextColor(dc, RGB(220, 220, 220));
		LPCSTR code = window.name;
		DrawText(dc, code, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		return CDRF_SKIPDEFAULT;
		break;
	}
	case CDDS_PREERASE: {
		printf("CDDS_PREERASE\n");
		break;
	}
	case CDDS_POSTPAINT: {
		printf("CDDS_POSTPAINT\n");
		break;
	}
	case CDDS_POSTERASE: {
		printf("CDDS_POSTERASE\n");
		break;
	}
	}
	return CDRF_DODEFAULT;
}
void OnTTShow(NMHDR* hdr) {
	HDC dc = ((NMTTCUSTOMDRAW*)hdr)->nmcd.hdc;
	HFONT font = (HFONT)SendMessage(hdr->hwndFrom, WM_GETFONT, 0,0);
	HFONT preFont = (HFONT)SelectObject(dc, font);
	TCHAR text[256];
	TOOLINFO info{};
	info.cbSize = sizeof(info);
	info.hwnd = GetParent(tooltip.hwnd);
	info.uId = (UINT_PTR)GetParent(tooltip.hwnd);
	info.lpszText = text;
	SendMessage(tooltip.hwnd, TTM_GETTEXTA, 256, (LPARAM)(LPTOOLINFO)&info);

	RECT rc;
	GetWindowRect(hdr->hwndFrom, &rc);
	LRESULT res = SendMessage(hdr->hwndFrom, TTM_ADJUSTRECT, TRUE, (LPARAM)&rc);
	
	SetWindowPos(hdr->hwndFrom,
		NULL,
		rc.left, rc.top,
		0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

LRESULT ToolTipNotify(NMHDR* hdr) {
	//TTINFO
	switch (hdr->code)
	{
	//case TTN_GETDISPINFO:
	//	printf("TTN_GETDISINFO\n");
	//	break;
	case TTN_NEEDTEXT:
		printf("TTN_NEEDTEXT\n");
		break;
	case TTN_POP:
		printf("TTN_POP\n");
		break;
	case TTN_SHOW:
		printf("TTN_SHOW\n");
		OnTTShow(hdr);
		return TRUE;
		break;
	case NM_CUSTOMDRAW:
		printf("NM_CUSTOMDRAW\n");
		return TooTipCustomDraw((NMTTCUSTOMDRAW*)hdr);
		break;
	default:
		break;
	}
	return CDRF_DODEFAULT;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	RECT rc;
	TCHAR code[256];
	Window* wnd = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (msg)
	{
	case WM_NCCREATE: {
		wnd = (Window*)((LPCREATESTRUCT)lp)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)wnd);
		break;
	}
	case WM_CREATE: {
		if (&window == wnd) break;
		printf("%s is created\n", wnd->name);
		tooltip.hwnd = CreateWindowEx(
			0,
			WC_TOOLTIP, 
			NULL,
			//WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
			//WS_POPUP | TTS_NOPREFIX,
			TTS_NOPREFIX,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hwnd, NULL, GetModuleHandle(NULL), NULL);
		
		TOOLINFO info{};
		info.cbSize = sizeof(info);
		info.uFlags = TTF_SUBCLASS;
		//info.uFlags = TTF_SUBCLASS | TTF_IDISHWND | TTF_TRANSPARENT;
		info.hwnd = hwnd;
		info.hinst = GetModuleHandle(NULL);
		info.uId = (UINT_PTR)hwnd;
		info.lpszText = (LPSTR)child.name;
		
		GetClientRect(hwnd, &info.rect);

		printf("Add Tool hwnd: %d\n", info.uId);
		SendMessage(tooltip.hwnd, TTM_ADDTOOL, 0, (LPARAM)&info);
		SendMessage(tooltip.hwnd, TTM_SETTIPBKCOLOR, (WPARAM)RGB(100, 0, 0), 0);
		SendMessage(tooltip.hwnd, TTM_SETMAXTIPWIDTH, 0, 130);
		break;
	}
	case WM_NOTIFY: {
		LPNMHDR hdr = (LPNMHDR)lp;
		if (hdr->hwndFrom != tooltip.hwnd) break;
		return ToolTipNotify(hdr);
		//if (hdr->hwndFrom == tooltip.hwnd) {
		//	switch (hdr->code)
		//	{
		//	case TTN_SHOW: {
		//		printf("TTN_SHOW\n");
		//		break;
		//	}
		//	case NM_CUSTOMDRAW: {
		//		printf("tool tips custom draw\n");
		//		NMTTCUSTOMDRAW* cd = (NMTTCUSTOMDRAW*)hdr;
		//		if (cd->nmcd.dwDrawStage == CDDS_PREPAINT) {
		//			COLORREF clr = (COLORREF)SendMessage(tooltip.hwnd,
		//				TTM_GETTIPBKCOLOR, 0, 0);
		//			SetTextColor(cd->nmcd.hdc, clr);
		//			SetBkColor(cd->nmcd.hdc, clr);
		//			return CDRF_NOTIFYPOSTPAINT;
		//		}
		//		if (cd->nmcd.dwDrawStage == CDDS_POSTPAINT) {
		//			SetTextColor(cd->nmcd.hdc, RGB(0, 0, 0));
		//			RECT rc = cd->nmcd.rc;
		//			TextOut(cd->nmcd.hdc, rc.left, rc.top, "Hello world", 12);
		//			return CDRF_SKIPDEFAULT;
		//		}
		//		return CDRF_DODEFAULT;
		//	}
		//	}
		//	
		//}
		break;
	}

	case WM_PAINT: {
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		LOGBRUSH lbr;
		GetObject(wnd->br, sizeof(LOGBRUSH), &lbr);
		COLORREF clr = lbr.lbColor;
		Wnd32::DrawFillRect(dc, ps.rcPaint, clr);

		EndPaint(hwnd, &ps);
		break;
	}
	case WM_SIZE: {
		//if (hwnd != button.hwnd) break;
		SIZE size{ LOWORD(lp), HIWORD(lp) };
		printf("size %d %d\n", size.cx, size.cy);
		int g = 19;
		break;
	}

	}
	//if (wnd->hwnd == tooltip.hwnd)
	//	return DefSubclassProc(hwnd, msg, wp, lp);
	return DefWindowProc(hwnd, msg, wp, lp);
}
