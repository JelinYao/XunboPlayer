// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
#define _CRT_SECURE_NO_WARNINGS	1
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <time.h>
#include <CommDlg.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
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

//��������ص�ַ
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