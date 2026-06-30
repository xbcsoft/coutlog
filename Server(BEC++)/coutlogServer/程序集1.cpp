#include "stdafx.h"

StrW 取SYSTEMTIME文本()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	return sprintF<W>(L":%02d:%02d.%03d", st.wMinute, st.wSecond, st.wMilliseconds);
}

void 定位文件(StrX filepath)
{
	StrW arg = L"/select,\"" + filepath + L"\"";
	运行(L"explorer.exe", arg);
}

COLORREF _LightenColor(COLORREF col, double factor) {
	int r = GetRValue(col);
	int g = GetGValue(col);
	int b = GetBValue(col);
	r = (int)(r + (255 - r) * factor);
	g = (int)(g + (255 - g) * factor);
	b = (int)(b + (255 - b) * factor);
	return RGB(r, g, b);
}

COLORREF _DarkenColor(COLORREF col, double factor) {
	int r = (int)(GetRValue(col) * (1.0 - factor));
	int g = (int)(GetGValue(col) * (1.0 - factor));
	int b = (int)(GetBValue(col) * (1.0 - factor));
	return RGB(r, g, b);
}