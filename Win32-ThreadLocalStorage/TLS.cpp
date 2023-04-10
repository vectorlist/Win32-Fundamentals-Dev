#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <vector>
#include <strsafe.h>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <../Common/Log.h>
#include <../Common/Wnd32.h>
#include <memory>
#include <assert.h>

void ErrorExit(LPCSTR code) {
	fprintf(stderr, "%s\n", code);
	ExitProcess(0);
}

struct Wnd
{
	HWND hwnd;
	Wnd() : hwnd(nullptr) {}
};

struct TLS
{
	Wnd* wnd;
	HWND mainHwnd;
	HHOOK msgHook;
	TLS() : wnd(nullptr), mainHwnd(nullptr), msgHook(nullptr) {}
};

typedef std::shared_ptr<TLS> TLSPtr;

typedef struct Application
{
	Application() {
		mTlsIndex = TlsAlloc();
		if (mTlsIndex == TLS_OUT_OF_INDEXES) {
			ErrorExit("uut of TLS index");
		}
	}
	~Application() {
		mTlsList.clear();
		if (mTlsIndex != TLS_OUT_OF_INDEXES) {
			::TlsSetValue(mTlsIndex, NULL);
			::TlsFree(mTlsIndex);
		}
	}
	
	void SetTlsData() {
		TLS* data = GetTLS();
		if (data == NULL) {
			data = new TLS;
			TLSPtr dataPtr(data);

			mTlsList.push_back(dataPtr);
			assert(::TlsSetValue(mTlsIndex, data));
		}
	}

	TLS* GetTLS() const {
		return (TLS*)TlsGetValue(mTlsIndex);
	}
	DWORD mTlsIndex;
	std::vector<TLSPtr> mTlsList;
}App;

#define WC_WINDOW	"WC_WINDOW"

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);


HWND mainHwnd;

int main(int args, char* argv[])
{
	App app;

	InitCommonControls();
	HINSTANCE hInst = GetModuleHandle(NULL);
	WNDCLASSEX wc{};
	wc.cbSize        = sizeof(wc);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hInstance     = hInst;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = WC_WINDOW;
	wc.style         = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc   = WndProc;
	GetClassInfoEx(wc.hInstance, wc.lpszClassName, &wc);
	RegisterClassEx(&wc);

	mainHwnd = CreateWindowEx(NULL, wc.lpszClassName, WC_WINDOW,
		WS_POPUP | WS_VISIBLE, 600, 400, 700, 200, nullptr, 0, wc.hInstance, NULL);

	
	ShowWindow(mainHwnd, TRUE);
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
	return DefWindowProc(hwnd, msg, wp, lp);
}

