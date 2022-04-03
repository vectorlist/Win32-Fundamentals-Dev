#include "DlgTemplate.h"
#include <string>

DlgTemplate::DlgTemplate(LPCSTR caption, DWORD style, int x, int y, int cx, int cy, LPCTSTR font, WORD fontSize)
{
	usedBufferLen = sizeof(DLGTEMPLATE);
	totalBufferLen = usedBufferLen;
	mData = (DLGTEMPLATE*)malloc(totalBufferLen);

	mData->style = style;
	if (font != NULL)
		mData->style |= DS_SETFONT;
	mData->x = x;
	mData->y = y;
	mData->cx = cx;
	mData->cy = cy;
	mData->cdit = 0;
	mData->dwExtendedStyle = 0;

	const char* name = "\0";
	AppendData((LPVOID)name, 2);
	AppendData((LPVOID)name, 2);

	AppendString(caption);

	if (font != NULL) {
		AppendData(&fontSize, sizeof(DWORD));
		AppendString(font);
	}

}

DlgTemplate::~DlgTemplate()
{
	free(mData);
}

void DlgTemplate::AppendData(void* data, int len)
{
	EnsureSpace(len);
	memcpy((char*)mData + usedBufferLen, data, len);
	usedBufferLen += len;
}

void DlgTemplate::EnsureSpace(int len)
{
	if (len + usedBufferLen > totalBufferLen) {
		totalBufferLen += len * 2;
		void* newBuffer = malloc(totalBufferLen);
		memcpy(newBuffer, mData, usedBufferLen);
		free(mData);
		mData = (LPDLGTEMPLATE)newBuffer;
	}
}

void DlgTemplate::AppendString(LPCTSTR code)
{
	int len = MultiByteToWideChar(CP_ACP, 0, code, -1, NULL, 0);
	WCHAR* wCode = (WCHAR*)malloc(sizeof(WCHAR) * len);
	MultiByteToWideChar(CP_ACP, 0, code, -1, wCode, len);
	AppendData(wCode, sizeof(WCHAR) * len);
	free(wCode);
}

DlgTemplateEx::DlgTemplateEx(UINT style, UINT x, UINT y, UINT cx, UINT cy,
	LPCSTR lpszMenu, LPCSTR lpszClass, LPCSTR lpszTitle)
{
	mSize = sizeof(DLGTEMPLATE);
	mCurrentSize = mSize;
	
	UINT menuSize = sizeof(lpszMenu);
	mCurrentSize += menuSize;
	UINT ctlSize = sizeof(lpszClass);
	mCurrentSize += ctlSize;
	
	UINT nMenuStrSize = MultiByteToWideChar(CP_ACP, 0, lpszMenu, -1, NULL, 0);

	UINT nClassStrSize = MultiByteToWideChar(CP_ACP, 0, lpszClass, -1, NULL, 0);
	
}

DlgTemplateBase::DlgTemplateBase(LPCSTR title, UINT style, UINT x, UINT y, UINT cx, UINT cy)
{
	DLGTEMPLATEINFO info;
	info.style = style;
	info.x = x;
	info.y = y;
	info.cx = cx;
	info.cy = cy;
	info.dwExtendedStyle = NULL;
	info.cdit = 0;
	info.menu = 0x0000;
	info.className = 0x0000;

	UINT nByte = sizeof(info);
	
	memcpy_s(m_pBuffer + m_uBufferOffset, DLGTEMPLATE_MAX_BUFFER, (void*)&info, nByte);
	m_uBufferOffset += nByte;


	AppendString(title);
	WORD size = 8;
	AppendByte(&size, sizeof(size));
	LPCSTR font = "Tahoma";
	std::wstring wFont(font, font + strlen(font));
	AppendString(font);
}


BOOL DlgTemplateBase::AddItem(LPCSTR text, DWORD style, WORD ctrl, UINT x, UINT y, UINT cx, UINT cy, WORD id)
{
	DLGITEMTEMPLATEINFO info{};
	info.style = style;
	info.dwExtendedStyle = 0;
	info.x = x;
	info.y = y;
	info.cx = cx;
	info.cy = cy;
	info.id = id;
	info.ctrlType = 0xFFFF;
	info.ctrl = ctrl;

	//the DLGITEMTEMPLATE structure must be aligned on DWORD(4byte) boundary
	UINT size = sizeof(DWORD);
	UINT alignment = m_uBufferOffset % size; //alignment by 2bytes
	UINT a = 50 % 4;

	if (alignment != 0)
		m_uBufferOffset += alignment;

	AppendByte(&info, sizeof(info));

	AppendString(text);

	WORD dummy = 0;
	AppendByte(&dummy, sizeof(WORD));

	DLGTEMPLATE* pDlgTemplate = (LPDLGTEMPLATE)&m_pBuffer;
	pDlgTemplate->cdit++;


	return 0;
}

bool DlgTemplateBase::AppendByte(LPVOID data, UINT size)
{
	if (memcpy_s(m_pBuffer + m_uBufferOffset, DLGTEMPLATE_MAX_BUFFER, data, size) == 0) {
		m_uBufferOffset += size;
		return true; //success
	}
	return false;
}

bool DlgTemplateBase::AppendString(LPCSTR code)
{
	UINT codeLength = MultiByteToWideChar(CP_ACP, 0, code, -1, NULL, 0);
	UINT nByte = codeLength * sizeof(WCHAR);
	WCHAR* wideTitle = (WCHAR*)malloc(nByte);
	MultiByteToWideChar(CP_ACP, 0, code, -1, wideTitle, codeLength);

	memcpy_s(m_pBuffer + m_uBufferOffset, DLGTEMPLATE_MAX_BUFFER, (void*)wideTitle, nByte);
	m_uBufferOffset += nByte;
	free(wideTitle);
	return false;
}
