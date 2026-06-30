#pragma once

#pragma region 全局量区(SymbolsG)
#include "R.h"
#pragma endregion

#pragma region BEWin32UI
//#version 0.6
#include <BEWin32UI/BEWin32UI.h>
#ifndef _LIB
#ifdef _DEBUG
#pragma comment(lib,"BEWin32UI/Debug/BEWin32UI.lib")
#else
#pragma comment(lib,"BEWin32UI/Release/BEWin32UI.lib")
#endif
#endif
#pragma endregion

#pragma region 扩展界面库一
//#version 1.0
#include <扩展界面库一/扩展界面库一.h>
#ifndef _LIB
#ifdef _DEBUG
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"扩展界面库一/x64/Debug/扩展界面库一.lib")
#else
#pragma comment(lib,"扩展界面库一/x64/Debug/扩展界面库一_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"扩展界面库一/Debug/扩展界面库一.lib")
#else
#pragma comment(lib,"扩展界面库一/Debug/扩展界面库一_static.lib")
#endif
#endif
#else
#ifdef _WIN64
#ifdef _DLL
#pragma comment(lib,"扩展界面库一/x64/Release/扩展界面库一.lib")
#else
#pragma comment(lib,"扩展界面库一/x64/Release/扩展界面库一_static.lib")
#endif
#else
#ifdef _DLL
#pragma comment(lib,"扩展界面库一/Release/扩展界面库一.lib")
#else
#pragma comment(lib,"扩展界面库一/Release/扩展界面库一_static.lib")
#endif
#endif
#endif
#endif
#pragma endregion

#pragma region 超级列表框扩展编辑
//#version 0.0
#include "超级列表框扩展编辑/超级列表框扩展编辑.h"
#pragma endregion

#pragma region HTTPNative
//#version 1.0
#include "HTTPNative/HTTPNative.h"
#pragma endregion

#pragma region 信息框增强版
//#version 0.0
#include "信息框增强版/信息框增强版.h"
#pragma endregion

