#include "Log.h"
#include <string>
#include <vector>

LPCSTR LOG::ClassStyle(UINT style)
{
	static std::basic_string<TCHAR> code;
	code.clear();
	std::vector<std::string> list;
	if (style & CS_VREDRAW)
		list.emplace_back("CS_VREDRAW");
	if (style & CS_HREDRAW)
		list.emplace_back("CS_HREDRAW");
	if (style & CS_DBLCLKS)
		list.emplace_back("CS_DBLCLKS");
	if (style & CS_OWNDC)
		list.emplace_back("CS_OWNDC");
	if (style & CS_CLASSDC)
		list.emplace_back("CS_CLASSDC");
	if (style & CS_PARENTDC)
		list.emplace_back("CS_PARENTDC");
	if (style & CS_NOCLOSE)
		list.emplace_back("CS_NOCLOSE");
	if (style & CS_SAVEBITS)
		list.emplace_back("CS_SAVEBITS");
	if (style & CS_BYTEALIGNCLIENT)
		list.emplace_back("CS_BYTEALIGNCLIENT");
	if (style & CS_BYTEALIGNWINDOW)
		list.emplace_back("CS_BYTEALIGNWINDOW");
	if (style & CS_GLOBALCLASS)
		list.emplace_back("CS_GLOBALCLASS");

	for (int i = 0; i < list.size(); i++) {
		if (i + 1 == list.size())
			code.append(list[i]);
		else
			code.append(list[i]).append(" | ");
	}
	return code.c_str();
}
