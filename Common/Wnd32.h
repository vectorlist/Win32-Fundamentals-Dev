#pragma once
#include <Windows.h>

namespace Wnd32 {

	inline void DrawFillRect(HDC dc, const RECT& rc, COLORREF clr) {
		::SetBkColor(dc, clr);
		::ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	}

	inline LPCWSTR StrToWideStr(LPCSTR code) {
		static std::wstring tempStr;
		tempStr.clear();
		tempStr.assign(code, code + strlen(code));
		return tempStr.c_str();
	}

	inline UINT GetChildsNum(HWND hwnd) {
		UINT num = 0;
		HWND child = GetWindow(hwnd, GW_CHILD);
		if (!child) return num;
		//num++;
		HWND nextChild = child;
		while (nextChild != NULL)
		{
			nextChild = GetNextWindow(nextChild, GW_HWNDNEXT);
			num++;
		}
		return num;
	}

	inline BOOL GetChildHwndList(HWND src, UINT size, HWND* dst) {
		HWND child = GetWindow(src, GW_CHILD);
		if (!child) return FALSE;
		for (int i = 0; i < size; i++) {
			dst[i] = child;
			child = GetNextWindow(child, GW_HWNDNEXT);
		}
		return TRUE;
	}

	inline LPCSTR GetHwndText(HWND hwnd) {
		static std::basic_string<TCHAR> temp;
		auto len = GetWindowTextLength(hwnd);
		temp.reserve(len + 1);
		GetWindowText(hwnd, (LPSTR)temp.c_str(), len + 1);
		return temp.c_str();
	}

	inline RECT GetLocalCoordRect(HWND hwnd) {
		RECT rc;
		GetWindowRect(hwnd, &rc);
		MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&rc, 2);
		return rc;
	}

}