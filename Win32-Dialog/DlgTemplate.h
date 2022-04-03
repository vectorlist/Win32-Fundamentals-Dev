#pragma once
#include <Windows.h>

class DlgTemplate
{
public:
	DlgTemplate(LPCSTR caption, DWORD style, int x, int y,
		int cx, int cy, LPCTSTR font = NULL, WORD fontSize = 8);
	~DlgTemplate();

	void AppendData(void* data, int len);
	void EnsureSpace(int len);
	void AppendString(LPCTSTR code);
	int usedBufferLen;
	int totalBufferLen;
	LPDLGTEMPLATE mData;
	operator LPDLGTEMPLATE() const { return mData; }
};

class DlgCreateInfo : DLGTEMPLATE
{
public:
	
	CHAR* menuClass;
	CHAR* ctlClass;
	LPCSTR text;
	

};

#define NULL_CHAR "\0"

class DlgTemplateEx
{
public:
	DlgTemplateEx(UINT style, UINT x, UINT y, UINT cx, UINT cy,
		LPCSTR lpszMenu, LPCSTR lpszClass, LPCSTR lpszTitle);

	UINT mCurrentSize;
	UINT mSize;
	LPDLGTEMPLATE mData;
	enum {
		BufferSize = 4096 * 16
	};
};

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
	};

	struct DLGITEMTEMPLATEINFO : DLGITEMTEMPLATE
	{
		//Value	Meaning
		//0x0080	Button
		//0x0081	Edit
		//0x0082	Static
		//0x0083	List box
		//0x0084	Scroll bar
		//0x0085	Combo box
		WORD ctrlType;
		WORD ctrl;
	};

	UINT8	m_pBuffer[DLGTEMPLATE_MAX_BUFFER];
	UINT	m_uBufferOffset = 0;
	operator LPDLGTEMPLATE() const { return (LPDLGTEMPLATE)m_pBuffer; }
	BOOL AddItem(LPCSTR text, DWORD style, WORD ctrl,
		UINT x, UINT y, UINT cx, UINT cy, WORD id);
	bool AppendByte(LPVOID data, UINT size);
	bool AppendString(LPCSTR code);
};

struct DLGTemplate
{

};

struct DLGTemplateItem
{

};

//void DlgTemplate::AppendData(void* data, int len)
//{
//	EnsureSpace(len);
//	memcpy((char*)mData + usedBufferLen, data, len);
//	usedBufferLen += len;
//}
//
//void DlgTemplate::EnsureSpace(int len)
//{
//	if (len + usedBufferLen > totalBufferLen) {
//		totalBufferLen += len * 2;
//		void* newBuffer = malloc(totalBufferLen);
//		memcpy(newBuffer, mData, usedBufferLen);
//		free(mData);
//		mData = (LPDLGTEMPLATE)newBuffer;
//	}
//}
//
//void DlgTemplate::AppendString(LPCTSTR code)
//{
//	int len = MultiByteToWideChar(CP_ACP, 0, code, -1, NULL, 0);
//	WCHAR* wCode = (WCHAR*)malloc(sizeof(WCHAR) * len);
//	MultiByteToWideChar(CP_ACP, 0, code, -1, wCode, len);
//	AppendData(wCode, sizeof(WCHAR) * len);
//	free(wCode);
//}