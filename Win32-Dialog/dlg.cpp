#include "dlg.h"

DialogTemplate::DialogTemplate(const wchar_t * caption, u32 x, u32 y, u32 width, u32 height)
	: _size(0)
{
	Header header = { 0 };

	header.dlg.style = DS_SETFONT | DS_CENTER | WS_POPUP | WS_CAPTION;
	header.dlg.dwExtendedStyle = 0;

	header.dlg.x = x;
	header.dlg.y = y;
	header.dlg.cx = width;
	header.dlg.cy = height;

	header.dlg.cdit = 0;

	header.menu = 0x0000;
	header.className = 0x0000;

	append(header);
	appendString(caption);

	wchar_t font[] = L"Tahoma";
	WORD fontSize = 8;
	append(fontSize);
	appendString(font);
}

void DialogTemplate::button(const wchar_t * caption, u32 x, u32 y, u32 width, u32 height, WORD id)
{
	add(Button, caption, WS_TABSTOP | WS_CHILD | WS_VISIBLE, 0, x, y, width, height, id);
}

void DialogTemplate::groupbox(const wchar_t * caption, u32 x, u32 y, u32 width, u32 height, WORD id)
{
	add(Button, caption, BS_GROUPBOX | WS_TABSTOP | WS_CHILD | WS_VISIBLE, 0, x, y, width, height, id);
}

void DialogTemplate::editboxro(const wchar_t * caption, u32 x, u32 y, u32 width, u32 height, WORD id)
{
	add(Edit, caption, ES_AUTOHSCROLL | ES_READONLY | ES_LEFT | WS_BORDER | WS_CHILD | WS_VISIBLE, 0, x, y, width, height, id);
}

void DialogTemplate::text(const wchar_t * caption, u32 x, u32 y, u32 width, u32 height, WORD id)
{
	add(Static, caption, ES_LEFT | WS_CHILD | WS_VISIBLE, 0, x, y, width, height, id);
}

void DialogTemplate::add(WORD type, const  wchar_t * caption, DWORD style, DWORD styleEx, u32 x, u32 y, u32 width, u32 height, WORD id)
{
	Item item = { 0 };

	item.tpl.id = id;

	item.tpl.style = style;
	item.tpl.dwExtendedStyle = styleEx;

	item.tpl.x = x;
	item.tpl.y = y;
	item.tpl.cx = width;
	item.tpl.cy = height;

	item.preType = 0xFFFF;
	item.type = type;

	align(sizeof(DWORD));

	append(item);
	appendString(caption);

	WORD dummy = 0;
	append(dummy);

	Header * hdr = (Header *)&_buffer;
	hdr->dlg.cdit++;
}

void DialogTemplate::align(uint size)
{
	u32 pad = _size % size;
	if (0 != pad)
	{
		_size += pad;
	}
}

bool DialogTemplate::appendString(const wchar_t * str)
{
	size_t len = (::wcslen(str) + 1) * sizeof(wchar_t);
	return append(str, len);
}

template<class T>
bool DialogTemplate::append(const T & data)
{
	return append(&data, sizeof(T));
}

bool DialogTemplate::append(const void * data, uint len)
{
	bool result = 0 == ::memcpy_s(&_buffer[_size], BufferSize, data, len);
	_size += len;
	return result;
}

DLGTEMPLATE * DialogTemplate::get() const
{
	return (DLGTEMPLATE *)&_buffer;
}