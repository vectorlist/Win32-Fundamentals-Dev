#include "DlgTemplate.h"
#include <string>

DlgTemplateBase::DlgTemplateBase(LPCSTR title, UINT style, UINT x, UINT y, UINT cx, UINT cy)
{
	
	DLGTEMPLATEINFO info = {};
	info.style = style;
	info.style |= DS_SETFONT;
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
	WORD size = 10;
	AppendByte(&size, sizeof(size));
	LPCSTR font = "Segoe UI";
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
	info.ctrl[0] = 0xFFFF;
	info.ctrl[1] = ctrl;

	//the DLGITEMTEMPLATE structure must be aligned on DWORD(4byte) boundary
	UINT size = sizeof(DWORD);
	UINT alignment = m_uBufferOffset % size; //alignment by 2bytes

	if (alignment != 0)
		m_uBufferOffset += alignment;

	AppendByte(&info, sizeof(DLGITEMTEMPLATEINFO));

	AppendString(text);

	WORD dummy = 0;
	AppendByte(&dummy, sizeof(WORD));

	DLGTEMPLATE* pDlgTemplate = (LPDLGTEMPLATE)&m_pBuffer;
	pDlgTemplate->cdit++;

	DLGItem item;
	item.hwnd = nullptr;
	item.id = id;
	m_items.emplace_back(item);
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
