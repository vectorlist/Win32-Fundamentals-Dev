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

#include <../Common/Log.h>
#include <../common.h>

#define WC_WINDOW	"WC_WINDOW"
#define WC_CHILD	"WC_CHILD"
#define DLGTITLE  "Debug"
#define DLGFONT   "MS Sans Serif"
#define DLGAPPLY  "&Apply"
#define DLGCANCEL "&Cancel"
#define NUMCHARS(aa) (sizeof(aa)/sizeof((aa)[0]))
#define IDC_BITMAP 99
//#pragma pack(push, 4)    
//static struct 
//{
//	DWORD style;
//	DWORD dwExStyle;
//	WORD ccontols;
//	short x;
//	short y;
//	short cx;
//	short cy;
//	WORD menu;
//	WORD windowClass;
//	CHAR wszSize[NUMCHARS(DLGTITLE)];
//	short pointSize;
//	CHAR wszFont[NUMCHARS(DLGFONT)];
//	// control info
////
//	struct {
//		DWORD  style;
//		DWORD  exStyle;
//		short  x;
//		short  y;
//		short  cx;
//		short  cy;
//		WORD   id;
//		WORD   sysClass;       // 0xFFFF identifies a system window class
//		WORD   idClass;        // ordinal of a system window class
//		CHAR  wszTitle[NUMCHARS(DLGAPPLY)];
//		WORD   cbCreationData; // bytes of following creation data
// //       WORD   wAlign;         // align next control to DWORD boundry.
//	} apply;
//
//	struct {
//		DWORD  style;
//		DWORD  exStyle;
//		short  x;
//		short  y;
//		short  cx;
//		short  cy;
//		WORD   id;
//		WORD   sysClass;       // 0xFFFF identifies a system window class
//		WORD   idClass;        // ordinal of a system window class
//		CHAR  wszTitle[NUMCHARS(DLGCANCEL)];
//		WORD   cbCreationData; // bytes of following creation data
//	} cancel;
//
//	struct {
//		DWORD  style;
//		DWORD  exStyle;
//		short  x;
//		short  y;
//		short  cx;
//		short  cy;
//		WORD   id;
//		WORD   sysClass;       // 0xFFFF identifies a system window class
//		WORD   idClass;        // ordinal of a system window class
//		CHAR  wszTitle[1];    // title string or ordinal of a resource
//		WORD   cbCreationData; // bytes of following creation data
//	} bitmap;
//}dlgTemplate = {
//	   WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU  // style  0x94c800c4
//   | DS_MODALFRAME | DS_3DLOOK
//   | DS_SETFONT,
//   0x0,        // exStyle;
//   3,          // ccontrols
//   0, 0, 300, 180,
//   0,                       // menu: none
//   0,                       // window class: none
//   DLGTITLE,                // Window caption
//   8,                       // font pointsize
//   DLGFONT,
//		  {
//	  WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP | BS_DEFPUSHBUTTON,   // 0x50030001
//	  WS_EX_NOPARENTNOTIFY, // 0x4
//	  190,160,50,14,
//	  IDOK,
//	  0xFFFF, 0x0080, // button
//	  DLGAPPLY, 0,
//	  },
//
//	  {
//	  WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,    // 0x50010000
//	  WS_EX_NOPARENTNOTIFY, // 0x4
//	  244,160,50,14,
//	  IDCANCEL,
//	  0xFFFF, 0x0080, // button
//	  DLGCANCEL, 0,
//	  },
//
//	  {
//	  WS_CHILD | WS_VISIBLE | WS_GROUP | SS_LEFT,    // 0x50020000
//	  WS_EX_NOPARENTNOTIFY, // 0x4
//	  6,6,288,8,
//	  IDC_BITMAP,
//	  0xFFFF, 0x0082, // static
//	  "", 0,
//	  },
//};
//#pragma pack(pop)    

#include <DlgTemplate.h>

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
INT_PTR WINAPI DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);


struct WindowData
{
	HWND hwnd;
	LPCSTR name;
};

WindowData window;
WindowData dlg;


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

	//Main window
	window.name = "Main Window";
	window.hwnd = CreateWindowEx(NULL, wc.lpszClassName, window.name,
		WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN, 600, 200, 700, 480, nullptr, 0, wc.hInstance, &window);


	DlgTemplateBase dlgBase("Memory Dialog", WS_VISIBLE, 20, 20, 200, 100);
	dlgBase.AddItem("Memory Ctrl", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
	DlgTemplateBase::Button, 0, 0, 100, 40, 1);

	dlg.hwnd = CreateDialogIndirect(wc.hInstance, dlgBase,
		window.hwnd,(DLGPROC)DlgProc, (LONG)&dlgBase);

	DWORD err = GetLastError();
	LOG::LogLastError();
	ShowWindow(window.hwnd, TRUE);
	//ShowWindow(dlg.hwnd, TRUE);

	
	
	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.lParam;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	return DefWindowProc(hwnd, msg, wp, lp);
}

LPCSTR GetWindowText(HWND hwnd) {
	static std::string szTempStr;
	UINT len = GetWindowTextLength(hwnd);
	szTempStr.reserve(len);
	GetWindowText(hwnd, (LPSTR)szTempStr.c_str(), len);
	return szTempStr.c_str();
}
void PaintContorl(HWND hwnd, HDC dc, const RECT& rc)
{
	SetBkMode(dc, TRANSPARENT);
	//DrawFillRect(dc, rc, RGB(100, 100, 100));
	auto pen = CreatePen(PS_SOLID, 1, RGB(20, 20, 20));
	auto br = CreateSolidBrush(RGB(65, 65, 65));
	SelectObject(dc, br);
	SelectObject(dc, pen);
	Rectangle(dc, rc.left, rc.top, rc.right, rc.bottom);


	SetTextColor(dc, RGB(220, 220, 220));
	LPCSTR lpText = GetWindowText(hwnd);

	DrawText(dc, lpText, -1, (LPRECT)&rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	DeleteObject(pen);
	DeleteObject(br);
}
INT_PTR WINAPI DlgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	//printf("message : %s\n", LOG::WndMessage(msg));
	switch (msg)
	{
	case WM_NCCREATE: {
		printf("WM_NCCREATE\n");
		break;
	}
	case WM_INITDIALOG: {
		printf("WM_INITDIALOG\n");
		HINSTANCE inst = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
		HWND button = CreateWindowEx(NULL, WC_BUTTON, "button", WS_VISIBLE | WS_CHILD,
			180, 20, 120, 30, hwnd,
			(HMENU)33, inst, NULL);
		DWORD id = 0;
		DlgTemplateBase* base = (DlgTemplateBase*)lp;

		//UINT size = base->m_items.size();
		break;
	}
	case WM_NOTIFY: {
		HWND ctrl = ((LPNMHDR)lp)->hwndFrom;
		if (ctrl) {
			LPNMHDR hdr = ((LPNMHDR)lp);
			UINT id = hdr->idFrom;
			NMCUSTOMDRAW* cd = (LPNMCUSTOMDRAW)(LPNMHDR)lp;
			switch (cd->dwDrawStage)
			{

			case CDDS_PREPAINT: {
				PaintContorl(hdr->hwndFrom,cd->hdc, cd->rc);

				//return CDRF_SKIPDEFAULT | CDRF_SKIPPOSTPAINT;
				return true;
				break;
			}
			default:
				break;
			}

		}
		break;
	}
	case WM_SETFONT: {
		RECT rc;
		GetClientRect(hwnd, &rc);
		printf("WM_FONT\n");
		ShowWindow(hwnd, SW_SHOW);
		break;
	}
	case WM_PAINT: {
		printf("WM_PAINT\n");
		PAINTSTRUCT ps{};
		HDC dc = BeginPaint(hwnd, &ps);

		DrawFillRect(dc, ps.rcPaint, RGB(35, 35, 35));
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DRAWITEM: {
		
		LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lp;
		printf("id : drawItem %d", dis->CtlID);
		PaintContorl(dis->hwndItem,dis->hDC, dis->rcItem);
		break;
	}
	case WM_CLOSE: {
		EndDialog(hwnd, IDCANCEL);
		break;
	}
	case WM_NCHITTEST: {
		HRESULT hit = DefWindowProc(hwnd, msg, wp, lp);
		if (hit == HTCLIENT) {
			return HTCAPTION;
		}
		break;
	}
	}
	//return DefDlgProc(hwnd, msg, wp, lp);
	return FALSE;
}
