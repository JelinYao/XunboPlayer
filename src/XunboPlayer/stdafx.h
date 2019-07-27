// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
#define _CRT_SECURE_NO_WARNINGS	1
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <time.h>
#include <CommDlg.h>

// TODO: 在此处引用程序需要的其他头文件
#ifdef _DEBUG
//#	pragma comment(lib, "./cef/lib_Debug/libcef")
#if (_MSC_VER == 1600)
#	pragma comment(lib, "../../Duilib/lib/DuiLibD_MT100")
#	pragma comment(lib, "./cef/lib_Debug/libcef_dll_wrapper100")
#endif
#if (_MSC_VER == 1500)
#	pragma comment(lib, "../../Duilib/lib/DuiLibD_MT")
//#	pragma comment(lib, "./cef/lib_Debug/libcef_dll_wrapper90")
#endif
#if (_MSC_VER == 1800)
#	pragma comment(lib, "../../Duilib/lib/DuiLibD_MT120")
//#	pragma comment(lib, "./cef/lib_Debug/libcef_dll_wrapper120")
#endif
#else 
//#	pragma comment(lib, "./cef/lib_Release/libcef")
#if (_MSC_VER == 1600)
#	pragma comment(lib, "../../Duilib/lib100/DuiLib_MT")
#	pragma comment(lib, "./cef/lib_Release/libcef_dll_wrapper100")
#endif
#if (_MSC_VER == 1500)
#	pragma comment(lib, "../../Duilib/lib/DuiLib_MT")
//#	pragma comment(lib, "./cef/lib_Release/libcef_dll_wrapper90")
#endif
#if (_MSC_VER == 1800)
#	pragma comment(lib, "../../Duilib/lib/DuiLib_MT120")
//#	pragma comment(lib, "./cef/lib_Release/libcef_dll_wrapper120")
#endif
#endif


#include "DuiLib/UIlib.h"
using namespace DuiLib;

#include "Common/Common.h"

//解码库下载地址
//http://aplayer.open.xunlei.com/codecs.zip



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif