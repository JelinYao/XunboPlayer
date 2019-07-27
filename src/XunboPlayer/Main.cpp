// XunboPlayer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include <atlbase.h>
#include <atlhost.h>
#include "XunboPlayer.h"
#include "WndMain.h"
#include "SoftDefine.h"
#include "Global.h"
#include "MD5\md5.h"


CComModule _module;
bool ParseCmdLine(const wstring& strCmd, OUT wstring& strTitle, OUT wstring& strUrl);
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	CoInitialize(NULL);
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	_module.Init(NULL, hInstance);
	AtlAxWinInit();
	CPaintManagerUI::SetInstance(hInstance);
	CWndShadow::Initialize(hInstance);
	CGlobal::Instance()->Init();
	wstring strUrl, strTitle;
	if (strTitle.empty())
		strTitle = SOFT_NAME;
	CWndMain* pWnd = new CWndMain(strUrl, strTitle);
	pWnd->Create(NULL, strTitle.c_str(), UI_WNDSTYLE_FRAME, 0);
	pWnd->CenterWindow();
	pWnd->ShowWindow();
	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();
	CGlobal::Instance()->UnInit();
	CoUninitialize();
	return 0;
}