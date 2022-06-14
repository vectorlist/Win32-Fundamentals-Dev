#include <Windows.h>
#include <commctrl.h>
#include <windowsx.h>
#include <process.h>
#include <iostream>
#include <Uxtheme.h>
struct _WINDOW_HANDLES
{
	HWND Main;
	HWND ListView;
	HWND TabControl;
	HWND OutputWindow;
	HWND InputWindow;
	HWND SplitterProc;
} wh;
HINSTANCE ghInst;
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK TabControlProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SplitterProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
static WNDPROC TabControlWndProc;
static WNDPROC TabOutputWndProc;
static WNDPROC SplitterWndProc;
int main(int args, char* arv[])
{
	ghInst = GetModuleHandle(NULL);
	WNDCLASS wc;
	memset(&wc, 0, sizeof(WNDCLASS));
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = ghInst;
	wc.lpszClassName = "MainWindowClass";
	wc.lpfnWndProc = WindowProcedure;
	if (!RegisterClass(&wc))
		return -1;
	wh.Main = CreateWindowEx(NULL, "MainWindowClass", "GUI", 
		WS_OVERLAPPEDWINDOW | WS_EX_COMPOSITED, CW_USEDEFAULT, CW_USEDEFAULT,
		1300, 700, NULL, NULL, ghInst, NULL);
	ShowWindow(wh.Main, SW_SHOW);
	MSG msg;
	memset(&msg, 0, sizeof(MSG));
	while (GetMessageW((&msg), NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return 0;
}
VOID InitializeComponent(HWND hWnd)
{
	RECT rcMain;
	GetClientRect(hWnd, &rcMain);

	wh.ListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "", WS_CHILD | WS_VISIBLE | LVS_REPORT, 0, 0, rcMain.right, 180, hWnd, (HMENU)0, ghInst, NULL);
	ListView_SetExtendedListViewStyle(wh.ListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	LVCOLUMN lvc;
	memset(&lvc, 0, sizeof(LVCOLUMN));
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.cx = 125;
	lvc.pszText = (LPSTR)"Column 1";
	ListView_InsertColumn(wh.ListView, 0, &lvc);
	lvc.iSubItem = 1;
	lvc.cx = 125;
	lvc.pszText = (LPSTR)"Column 2";
	ListView_InsertColumn(wh.ListView, 1, &lvc);
	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.mask = LVIF_TEXT;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.pszText = (LPSTR)"Item 1";
	ListView_InsertItem(wh.ListView, &lvi);
	ListView_SetItemText(wh.ListView, 0, 1, (LPSTR)"Item 2");

	wh.TabControl = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TABCONTROL, "", WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE, 0, 196, rcMain.right, rcMain.bottom - 196, hWnd, (HMENU)0, ghInst, NULL);
	TCITEM tie;
	tie.mask = TCIF_TEXT;
	tie.pszText = (LPSTR)"Tab";
	TabCtrl_InsertItem(wh.TabControl, 0, &tie);

	TabControlWndProc = (WNDPROC)SetWindowLongPtr(wh.TabControl, GWLP_WNDPROC, (LONG_PTR)TabControlProc);

	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont = CreateFont(
		19, 0,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut,
		lf.lfCharSet, lf.lfOutPrecision, lf.lfClipPrecision,
		lf.lfQuality, lf.lfPitchAndFamily, NULL);
	SendMessage(wh.TabControl, WM_SETFONT, (WPARAM)hFont, TRUE);
	HFONT IFont = CreateFont(
		17, 0,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut,
		lf.lfCharSet, lf.lfOutPrecision, lf.lfClipPrecision,
		lf.lfQuality, lf.lfPitchAndFamily, NULL);

	wh.OutputWindow = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN, 0, 25, rcMain.right, rcMain.bottom - 251, wh.TabControl, NULL, ghInst, NULL);
	SendMessage(wh.OutputWindow, EM_SETREADONLY, TRUE, 0);
	SendMessage(wh.OutputWindow, WM_SETFONT, (WPARAM)hFont, TRUE);
	wh.InputWindow = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "", WS_CHILD | WS_VISIBLE, 0, rcMain.bottom - 227, rcMain.right, 28, wh.TabControl, NULL, ghInst, NULL);
	SendMessage(wh.InputWindow, WM_SETFONT, (WPARAM)IFont, TRUE);

	wh.SplitterProc = CreateWindowEx(NULL, WC_STATIC, "", 
		WS_CHILD | WS_BORDER | WS_VISIBLE | SS_NOTIFY | SS_CENTER,
		0, 181, rcMain.right, 17, hWnd, NULL, ghInst, NULL);
	SplitterWndProc = (WNDPROC)SetWindowLongPtr(wh.SplitterProc, GWLP_WNDPROC, (LONG_PTR)SplitterProc);
}
LRESULT CALLBACK SplitterProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL bSplitterMoving;
	RECT rcMain, rcListView;
	GetClientRect(wh.Main, &rcMain);
	GetClientRect(wh.ListView, &rcListView);
	switch (message)
	{
	case WM_SETCURSOR:
	{
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		SetWindowLongPtr(hwnd, DWLP_MSGRESULT, TRUE);
		return TRUE;
	}
	case WM_LBUTTONDOWN:
	{
		bSplitterMoving = TRUE;
		SetCapture(wh.SplitterProc);
		return 0;
	}
	break;
	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		bSplitterMoving = FALSE;
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		POINT  pt{ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
		ClientToScreen(wh.SplitterProc, &pt);
		ScreenToClient(wh.Main, &pt);
		auto newSplitterPos = pt.y;
		if (newSplitterPos <= 125 || newSplitterPos >= 350)
		{
			return 0;
		}

		if (bSplitterMoving)
		{
			POINT  pt{ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
			ClientToScreen(wh.SplitterProc, &pt);
			ScreenToClient(wh.Main, &pt);
			auto newSplitterPos = pt.y;

			MoveWindow(wh.SplitterProc, 0, newSplitterPos, rcMain.right, 17, TRUE);
			MoveWindow(wh.ListView, 0, 0, rcMain.right, newSplitterPos, TRUE);
			MoveWindow(wh.TabControl, rcMain.left, newSplitterPos + 15, rcMain.right, rcMain.bottom - (newSplitterPos + 15), TRUE);
			MoveWindow(wh.OutputWindow, 0, 25, rcMain.right, rcMain.bottom - (newSplitterPos + 71), TRUE);
			MoveWindow(wh.InputWindow, rcMain.left, rcMain.bottom - (newSplitterPos + 46), rcMain.right, 28, TRUE);
		}
		return 0;
	}
	}
	return CallWindowProc(SplitterWndProc, hwnd, message, wParam, lParam);
}
LRESULT CALLBACK TabControlProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(255, 255, 255));
		SetBkColor(hdcStatic, RGB(0, 0, 0));
		HBRUSH hbrBkgnd = CreateSolidBrush(RGB(0, 0, 0));
		return (INT_PTR)hbrBkgnd;
	}
	case WM_CTLCOLOREDIT:
	{
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(255, 255, 255));
		SetBkColor(hdcStatic, RGB(0, 0, 0));
		HBRUSH hbrBkgnd = CreateSolidBrush(RGB(0, 0, 0));
		return (INT_PTR)hbrBkgnd;
	}
	}
	return CallWindowProc(TabControlWndProc, hDlg, message, wParam, lParam);
}
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	RECT rcMain, rcListView, rcTabControl;
	GetClientRect(hWnd, &rcMain);
	GetClientRect(wh.ListView, &rcListView);
	GetClientRect(wh.TabControl, &rcTabControl);
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CREATE:
	{
		InitializeComponent(hWnd);
		break;
	}
	case WM_SIZE:
	{
		POINT  pt{ GET_X_LPARAM(lp),GET_Y_LPARAM(lp) };
		ClientToScreen(wh.SplitterProc, &pt);
		ScreenToClient(wh.Main, &pt);
		int newSplitterPos = pt.y;

		SetWindowPos(wh.ListView, NULL, rcMain.left, rcMain.left, rcMain.right, newSplitterPos - rcMain.bottom - 1, SWP_NOMOVE);
		ListView_SetColumnWidth(wh.ListView, 1, LVSCW_AUTOSIZE_USEHEADER);
		SetWindowPos(wh.TabControl, NULL, rcMain.left, newSplitterPos - (9 + rcTabControl.bottom + rcListView.bottom), rcMain.right, rcMain.bottom - (rcListView.bottom + 19), SWP_NOMOVE);
		MoveWindow(wh.OutputWindow, 0, 25, rcMain.right, rcMain.bottom - (rcListView.bottom + 75), TRUE);
		MoveWindow(wh.InputWindow, rcMain.left, rcMain.bottom - (rcListView.bottom + 51), rcMain.right, 28, TRUE);
		SetWindowPos(wh.SplitterProc, NULL, rcMain.left, newSplitterPos - (24 + rcTabControl.bottom + rcListView.bottom), LOWORD(lp), 17, SWP_NOMOVE);
		SetWindowText(wh.SplitterProc, "...");
		break;
	}
	}
	return DefWindowProcW(hWnd, msg, wp, lp);
}