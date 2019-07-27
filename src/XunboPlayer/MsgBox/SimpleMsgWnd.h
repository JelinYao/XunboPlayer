#pragma once


class CSimpleMsgWnd
	:public WindowImplBase
{
public:
	CSimpleMsgWnd();
	~CSimpleMsgWnd();
	virtual LPCTSTR GetWindowClassName(void)const	{ return L"MsgBoxWndClass"; }
	virtual CDuiString	GetSkinFolder()				{ return L"Skin"; }
	virtual CDuiString	GetSkinFile()				{ return L"MsgWnd_simple.xml"; }
	void	SetMsg(LPCTSTR lpMsg)			{ m_strMsg = lpMsg; }

protected:
	virtual void	InitWindow();
	virtual void	OnFinalMessage( HWND hWnd );
	virtual LRESULT	HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool	OnNotifyBtnClose(void* pParam);
	bool	OnNotifyBtnYes(void* pParam);

private:
	wstring		m_strMsg;
};