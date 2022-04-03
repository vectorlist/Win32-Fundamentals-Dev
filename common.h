#pragma once
#include <Windows.h>

inline void DrawFillRect(HDC dc, const RECT& rc, COLORREF clr) {
	::SetBkColor(dc, clr);
	::ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
}