#pragma once
#include <Windows.h>
typedef UINT u32;
typedef UINT uint;
typedef UINT8 u8;

class DialogTemplate
{
public:
	enum
	{
		BufferSize = 4096 * 16,
	};

	DialogTemplate(const wchar_t * caption, u32 x, u32 y, u32 width, u32 height);

	DLGTEMPLATE * get() const;

	void button(const wchar_t * caption, u32 x, u32 y, u32 width, u32 height, WORD id);
	void groupbox(const wchar_t * caption, u32 x, u32 y, u32 width, u32 height, WORD id);
	void editboxro(const wchar_t * caption, u32 x, u32 y, u32 width, u32 height, WORD id);
	void text(const wchar_t * caption, u32 x, u32 y, u32 width, u32 height, WORD id);

private:

	enum
	{
		Button = 0x0080,
		Edit = 0x0081,
		Static = 0x0082,
		ListBox = 0x0083,
		ScrollBar = 0x0084,
		ComboBox = 0x0085,
	};

	struct Header
	{
		DLGTEMPLATE dlg;
		WORD menu;
		WORD className;
	};

	struct Item
	{
		DLGITEMTEMPLATE tpl;
		WORD preType;
		WORD type;
	};

	void add(
		WORD type,
		const wchar_t * caption,
		DWORD style,
		DWORD styleEx,
		u32 x, u32 y,
		u32 width, u32 height,
		WORD id);

	void align(uint size);

	bool appendString(const wchar_t * str);

	template<class T>
	bool append(const T & data);
	bool append(const void * data, uint len);

private:
	u8 _buffer[BufferSize];
	uint _size;
};