#pragma once
#include "Player\IPlayerCallback.h"

class CWndPlayBar :
	public WindowImplBase
{
public:
	CWndPlayBar(HWND hMainWnd);
	~CWndPlayBar();
	void UpdateTotalTime(const wstring& strTotalTime);
	void UpdatePlayTime(const wstring& strPlayTime, int nPercent);
	void UpdatePos(int nWidth, int nHeight);
	void UpdateState(PlayerState state, int nPercent=0);
	void SetVolume(long nVolume);
	void MouseOver();
	void MouseLeave();

protected:
	virtual LPCTSTR		GetWindowClassName()const	{ return L"XbyMainWndClass"; }
	virtual CDuiString	GetSkinFolder()				{ return _T("PlayerSkin"); }
	virtual CDuiString	GetSkinFile()				{ return _T("WndPlayBar.xml"); }
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual void OnClick(TNotifyUI& msg);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	bool OnNotifySlider(void* lpParam);
	bool OnNotifySliderVolume(void* lpParam);

private:
	bool m_bMouseOver;
	bool m_bMouse;
	bool m_bPlay;
	HWND m_hMainWnd;
	CSliderUI*	m_pSliderPlay;
	CSliderUI*	m_pSliderVolume;
	CControlUI* m_pLblLeftTime;
	CControlUI* m_pLblRightTime;
	CControlUI* m_pHorVolume;
	CControlUI* m_pLblState;
	CControlUI* m_pBtnPlay;
	CControlUI* m_pBtnPause;
};

