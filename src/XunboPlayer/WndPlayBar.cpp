#include "stdafx.h"
#include "WndPlayBar.h"
#include "Resource.h"
#include "MsgDefine.h"
#include "Global.h"



CWndPlayBar::CWndPlayBar(HWND hMainWnd)
: m_pSliderPlay(NULL)
, m_pLblLeftTime(NULL)
, m_pLblRightTime(NULL)
, m_pHorVolume(NULL)
, m_pLblState(NULL)
, m_hMainWnd(hMainWnd)
, m_pSliderVolume(NULL)
, m_pBtnPlay(NULL)
, m_pBtnPause(NULL)
, m_bPlay(false)
, m_bMouse(false)
, m_bMouseOver(false)
{
}


CWndPlayBar::~CWndPlayBar()
{

}

void CWndPlayBar::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CWndPlayBar::InitWindow()
{
	SetIcon(IDI_XUNBOPLAYER);
	m_pSliderPlay = dynamic_cast<CSliderUI*>(m_PaintManager.FindControl(L"slider_play"));
	ASSERT(m_pSliderPlay);
	m_pSliderPlay->OnNotify += MakeDelegate(this, &CWndPlayBar::OnNotifySlider);
	m_pLblLeftTime = m_PaintManager.FindControl(L"lbl_left_time");
	ASSERT(m_pLblLeftTime);
	m_pLblRightTime = m_PaintManager.FindControl(L"lbl_right_time");
	ASSERT(m_pLblRightTime);
	m_pHorVolume = m_PaintManager.FindControl(L"volume");
	ASSERT(m_pHorVolume);
	m_pLblState = m_PaintManager.FindControl(L"lbl_state");
	ASSERT(m_pLblState);
	m_pSliderVolume = dynamic_cast<CSliderUI*>(m_PaintManager.FindControl(L"slider_volume"));
	ASSERT(m_pSliderVolume);
	m_pSliderVolume->OnNotify += MakeDelegate(this, &CWndPlayBar::OnNotifySliderVolume);
	m_pSliderVolume->SetValue(20);
	m_pBtnPlay = m_PaintManager.FindControl(L"btn_play");
	ASSERT(m_pBtnPlay);
	m_pBtnPause = m_PaintManager.FindControl(L"btn_pause");
	ASSERT(m_pBtnPause);
	long nVolume = CGlobal::Instance()->GetVolume();
	m_pSliderVolume->SetValue(nVolume);
}

void CWndPlayBar::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_pBtnPlay)
	{
		if (m_bPlay)
		{
			::PostMessage(m_hMainWnd, WM_MAIN_MSG_PLAY, 0, 1);
		}
	}
	else if (msg.pSender == m_pBtnPause)
	{
		if (m_bPlay)
		{
			::PostMessage(m_hMainWnd, WM_MAIN_MSG_PLAY, 0, 0);
		}
	}
	else
		__super::OnClick(msg);
}

LRESULT CWndPlayBar::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (uMsg)
	{
	case WM_MOUSEHOVER:
		m_bMouse = false;
		m_bMouseOver = true;
		break;
	case WM_MOUSELEAVE:
		m_bMouse = false;
		m_bMouseOver = false;
		break;
	case WM_MOUSEMOVE:
		m_bMouseOver = true;
		if (!m_bMouse)
		{
			TRACKMOUSEEVENT tme = { 0 };
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER | TME_LEAVE;
			tme.hwndTrack = m_hWnd;
			tme.dwHoverTime = 10;
			_TrackMouseEvent(&tme);
			m_bMouse = true;
		}
		break;
	default:
		break;
	}
	return 0;
}

bool CWndPlayBar::OnNotifySlider(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_VALUECHANGED)
	{
		if (m_bPlay)
		{
			int nValue = m_pSliderPlay->GetValue();
			::PostMessage(m_hMainWnd, WM_MAIN_MSG_PRO_PLAY, 0, nValue);
		}
		else
		{
			m_pSliderPlay->SetValue(0);
		}
	}
	return true;
}

bool CWndPlayBar::OnNotifySliderVolume(void* lpParam)
{
	TNotifyUI* pNotify = (TNotifyUI*)lpParam;
	if (pNotify->sType == DUI_MSGTYPE_VALUECHANGED)
	{
		int nValue = m_pSliderVolume->GetValue();
		CGlobal::Instance()->SetVolume(nValue);
		::PostMessage(m_hMainWnd, WM_MAIN_MSG_PRO_VOLUME, 0, nValue);
	}
	return true;
}

void CWndPlayBar::UpdateTotalTime(const wstring& strTotalTime)
{//文件打开成功
	if (m_pLblRightTime)
	{
		m_pLblRightTime->SetText(strTotalTime.c_str());
	}
	if (m_pSliderPlay)
	{
		m_pSliderPlay->SetThumbImage(L"bar\\scroll_play.png");
	}
	m_bPlay = true;
	m_pBtnPlay->SetVisible(false);
	m_pBtnPause->SetVisible();
}

void CWndPlayBar::UpdatePlayTime(const wstring& strPlayTime, int nPercent)
{
	if (m_pLblLeftTime)
	{
		m_pLblLeftTime->SetText(strPlayTime.c_str());
	}
	if (m_pSliderPlay)
	{
		m_pSliderPlay->SetValue(nPercent);
	}
}

void CWndPlayBar::UpdatePos(int nWidth, int nHeight)
{
	if (m_pHorVolume)
	{
		RECT rcPos = { nWidth - 150 - 10, 30, nWidth - 10, 30 + 24 };
		m_pHorVolume->SetPos(rcPos);
	}
}

void CWndPlayBar::UpdateState(PlayerState state, int nPercent)
{
	CDuiString strText;
	switch (state)
	{
	case Ps_Opening:
		strText = L"正在打开...";
		break;
	case Ps_Pausing:
		strText = L"正在暂停...";
		break;
	case Ps_Paused:
		strText = L"已暂停";
		m_pBtnPlay->SetVisible();
		m_pBtnPause->SetVisible(false);
		break;
	case Ps_Playing:
		strText = L"正在播放...";
		break;
	case Ps_Play:
		strText = L"播放中";
		m_pBtnPlay->SetVisible(false);
		m_pBtnPause->SetVisible();
		break;
	case Ps_Closing:
		strText = L"";
		break;
	case Ps_Finish:
		strText = L"";
		m_pSliderPlay->SetThumbImage(L"");
		m_pSliderPlay->SetValue(0);
		m_pLblLeftTime->SetText(L"00:00");
		m_pLblRightTime->SetText(L"00:00");
		m_bPlay = false;
		m_pBtnPlay->SetVisible();
		m_pBtnPause->SetVisible(false);
		break;
	case Ps_Begin:
		strText = L"开始播放";
		break;
	case Ps_Error:
		strText = L"播放失败";
		m_pSliderPlay->SetThumbImage(L"");
		m_pSliderPlay->SetValue(0);
		m_bPlay = false;
		m_pBtnPlay->SetVisible();
		m_pBtnPause->SetVisible(false);
		break;
	case Ps_Loading:
		if (nPercent < 0)
			nPercent = 0;
		if (nPercent>100)
			nPercent = 100;
		if (nPercent == 100)
			strText = L"播放中";
		else
			strText.Format(L"缓冲中：%d%%", nPercent);
		break;
	default:
		break;
	}
	m_pLblState->SetText(strText);
}

void CWndPlayBar::SetVolume(long nVolume)
{
	if (nVolume < 0)
		nVolume = 0;
	if (nVolume>100)
		nVolume = 100;
	m_pSliderVolume->SetValue(nVolume);
}

void CWndPlayBar::MouseOver()
{
	::ShowWindow(m_hWnd, SW_SHOW);
}

void CWndPlayBar::MouseLeave()
{
	if (!m_bMouseOver)
	{
		::ShowWindow(m_hWnd, SW_HIDE);
	}
}
