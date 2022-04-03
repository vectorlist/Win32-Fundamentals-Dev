#pragma once
#include <Windows.h>

#define DLGTEMPLATE_MAX_BUFFER 4096 * 16

struct DlgTemplateBase
{
	DlgTemplateBase(LPCSTR title, UINT style, UINT x, UINT y, UINT cx, UINT cy);

	struct DLGTEMPLATEINFO : DLGTEMPLATE
	{
		//DLGTEMPLATE[24Byte]Menu[2Byte]ClassName[2Byte]
		WORD menu;
		WORD className;
		//DLGTEMPLATE.style has DT_SETFONT
		//variable member would be 
	};

	struct DLGITEMTEMPLATEINFO : DLGITEMTEMPLATE
	{
		//ctrl type default 0xFFFF
		/*------- ctrl ---------*/
		//0x0080	Button
		//0x0081	Edit
		//0x0082	Static
		//0x0083	List box
		//0x0084	Scroll bar
		//0x0085	Combo box
		/*----------------------*/
		WORD ctrlType;
		WORD ctrl;
	};
	enum {
		Button = 0x0080,
	};
	UINT8	m_pBuffer[DLGTEMPLATE_MAX_BUFFER];
	UINT	m_uBufferOffset = 0;
	operator LPDLGTEMPLATE() const { return (LPDLGTEMPLATE)&m_pBuffer; }
	BOOL AddItem(LPCSTR text, DWORD style, WORD ctrl,
		UINT x, UINT y, UINT cx, UINT cy, WORD id);
	bool AppendByte(LPVOID data, UINT size);
	bool AppendString(LPCSTR code);
};

