// XunboPlayer.cpp : ����Ӧ�ó������ڵ㡣
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
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
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