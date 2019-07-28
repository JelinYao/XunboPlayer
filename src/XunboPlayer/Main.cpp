// XunboPlayer.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <atlbase.h>
#include <atlhost.h>
#include "XunboPlayer.h"
#include "WndMain.h"
#include "SoftDefine.h"
#include "Global.h"



CComModule _module;
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
	CWndMain* pWnd = new CWndMain(L"", SOFT_NAME);
	pWnd->Create(NULL, SOFT_NAME, UI_WNDSTYLE_FRAME, 0);
	pWnd->CenterWindow();
	pWnd->ShowWindow();
	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();
	CGlobal::Instance()->UnInit();
	CoUninitialize();
	return 0;
}