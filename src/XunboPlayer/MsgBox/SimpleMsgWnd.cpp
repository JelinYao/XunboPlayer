#include "stdafx.h"
#include "SimpleMsgWnd.h"


CSimpleMsgWnd::CSimpleMsgWnd()
	: m_strMsg(L"")
{

}

CSimpleMsgWnd::~CSimpleMsgWnd()
{

}

void CSimpleMsgWnd::InitWindow()
{
	CControlUI* pControl = m_PaintManager.FindControl(L"btn_ok");
	if ( pControl )
		pControl->OnNotify += MakeDelegate(this, &CSimpleMsgWnd::OnNotifyBtnYes);
	pControl = m_PaintManager.FindControl(L"btn_close");
	if ( pControl )
		pControl->OnNotify += MakeDelegate(this, &CSimpleMsgWnd::OnNotifyBtnClose);
	pControl = m_PaintManager.FindControl(L"text_msg");
	if ( pControl )
		pControl->SetText(m_strMsg.c_str());
}

void CSimpleMsgWnd::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LRESULT CSimpleMsgWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYUP)
	{
		if (wParam == VK_ESCAPE)
		{
			Close();
		}
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

bool CSimpleMsgWnd::OnNotifyBtnClose(void* pParam)
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pParam;
	if(pNotifyUI->sType != DUI_MSGTYPE_CLICK) 
		return true;
	::PostMessage(m_hWnd, WM_CLOSE, IDCLOSE, 0);
	return true;
}

bool CSimpleMsgWnd::OnNotifyBtnYes( void* pParam )
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pParam;
	if(pNotifyUI->sType != DUI_MSGTYPE_CLICK) 
		return true;
	::PostMessage(m_hWnd, WM_CLOSE, IDYES, 0);
	return true;
}


