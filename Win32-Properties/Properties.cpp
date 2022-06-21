#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <assert.h>
#include <../Common/Wnd32.h>

#define WC_MAIN		"WC_MAIN"
#define WC_PROP		"WC_PROP"
#define WIDTH		840
#define HEIGHT		460

HWND main_hwnd = nullptr;
HWND prop_hwnd = nullptr;
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT WINAPI PropertiesProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

#define WND_PROPERTY	"WND_PROPERTY"
typedef struct WNDPROPERTY{
	int margin;
	SIZE ss;
	SIZE fs;
}*LPWNDPROPERTY;

LPWNDPROPERTY GetProperty(HWND hwnd) {
	assert(::IsWindow(hwnd));
	return (LPWNDPROPERTY)GetProp(hwnd, WND_PROPERTY);
}

int main(int args, char* argv[])
{
	WNDCLASSEX wc{};
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_VREDRAW | CS_HREDRAW;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hInstance     = GetModuleHandle(0);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.lpfnWndProc   = WndProc;
	wc.lpszClassName = WC_MAIN;
	
	ATOM mainAtom = RegisterClassExA(&wc);

	wc.lpfnWndProc = PropertiesProc;
	wc.lpszClassName = WC_PROP;

	ATOM propAtom = RegisterClassExA(&wc);

	int x = (GetSystemMetrics(SM_CXSCREEN) - WIDTH) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - HEIGHT) / 2;

	main_hwnd = CreateWindowEx(NULL, WC_MAIN, WC_MAIN, WS_VISIBLE | WS_POPUP,
		x, y, WIDTH, HEIGHT, NULL, (HMENU)0, wc.hInstance, 0);

	WNDPROPERTY prop{}; //margin, spicing size, fixed size
	prop.margin = 16;
	prop.ss.cx = 8;
	prop.ss.cy = 24;
	prop_hwnd = CreateWindowEx(NULL, WC_PROP, WC_PROP, WS_VISIBLE | WS_CHILD,
		0, 0, 0, 0, main_hwnd, 0, wc.hInstance, &prop);

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
	CHAR buf[128];
	switch (msg)
	{
	case WM_PAINT: {
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		int margin = 0;
		if (prop_hwnd) {
			margin +=((LPWNDPROPERTY)GetProp(prop_hwnd, WND_PROPERTY))->margin;
		}
		RECT rc;
		GetClientRect(hwnd, &rc);
		rc.bottom = rc.top +margin;
		SetTextColor(dc, RGB(190, 190, 190));
		SetBkMode(dc, TRANSPARENT);
		sprintf(buf, "Margins : %d", margin);
		DrawText(dc, buf, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		EndPaint(hwnd, &ps);
		return 0;
	}break;
	case WM_KEYDOWN: {
		if (wp == VK_ESCAPE) DestroyWindow(hwnd);
	}break;
	case WM_NCDESTROY:
		PostQuitMessage(0);
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
LRESULT WINAPI PropertiesProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_NCCREATE: {
		LPWNDPROPERTY src = (LPWNDPROPERTY)((CREATESTRUCT*)lp)->lpCreateParams;
		LPWNDPROPERTY dst =  (LPWNDPROPERTY)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WNDPROPERTY));
		memcpy(dst, src, sizeof(WNDPROPERTY));
		::SetProp(hwnd, WND_PROPERTY, (HANDLE)dst);
		printf("Set Properties allocated heap : %d\n", (int)dst);

	}break;
	case WM_SIZE: {
		HWND parent = ::GetParent(hwnd);
		if (parent) {
			RECT rc;
			GetClientRect(parent, &rc);
			WNDPROPERTY* prop = (WNDPROPERTY*)::GetProp(hwnd, WND_PROPERTY);
			int margin = prop->margin;
			InflateRect(&rc, -margin, -margin);
			InvalidateRect(parent, NULL, FALSE);
			SetWindowPos(hwnd, HWND_TOP, rc.left, rc.top,
				rc.right - rc.left, rc.bottom - rc.top, NULL);
		}

	}break;
	case WM_PAINT: {
		
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);
		Wnd32::DrawFillRect(dc, ps.rcPaint, RGB(32, 32, 32));
		SetTextColor(dc, RGB(190, 190, 190));
		SetBkMode(dc, TRANSPARENT);
		RECT rc[2];
		rc[0] = ps.rcPaint;
		rc[1] = ps.rcPaint;
		
		SIZE ss = GetProperty(hwnd)->ss;
		InflateRect(&rc[0], -ss.cx, -ss.cy);
		Wnd32::DrawFillRect(dc, rc[0], RGB(55, 52, 52));
		DrawText(dc, "press esc to exit", -1, &rc[0], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		rc[1].bottom = rc[1].top + ss.cy;
		CHAR buf[128];
		sprintf(buf, "Spacing width : %d height : %d", ss.cx, ss.cy);
		DrawText(dc, buf, -1, &rc[1], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		EndPaint(hwnd, &ps);
	}break;
	case WM_NCDESTROY: {
		LPWNDPROPERTY prop = (LPWNDPROPERTY)::GetProp(hwnd, WND_PROPERTY);
		if (prop) {
			printf("Delete Properties allocated : %d\n", (int)prop);
			HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE, prop);
		}
		Sleep(2000);
	}break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
