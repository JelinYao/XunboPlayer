#include "stdafx.h"
#include "WndMain.h"
#include "MsgDefine.h"




LRESULT CWndMain::OnMsgSetPlayProgress(WPARAM wParam, LPARAM lParam)
{
	if (m_pPlayer)
	{
		long nCurPos = (long)((lParam / 100.0)*m_pPlayer->GetDuration());
		m_pPlayer->SetPlayPosition(nCurPos);
	}
	return 0;
}

LRESULT CWndMain::OnMsgSetVolumeProgress(WPARAM wParam, LPARAM lParam)
{
	if (m_pPlayer)
	{
		m_pPlayer->SetVolume(lParam);
	}
	return 0;
}

LRESULT CWndMain::OnMsgPlay(WPARAM wParam, LPARAM lParam)
{
	if (m_pPlayer)
	{
		(lParam == 1) ? m_pPlayer->Play() : m_pPlayer->Pause();
	}
	return 0;
}

void CWndMain::OnMouseOverPlayer(HWND hWnd)
{
	m_bMouse = false;
	if (m_pPlayBar && IsWindow(m_hPlayBarWnd))
		m_pPlayBar->MouseOver();
}

void CWndMain::OnMouseLeavePlayer(HWND hWnd)
{
	if (m_pPlayBar && IsWindow(m_hPlayBarWnd))
		m_pPlayBar->MouseLeave();
	if(m_bMouse) ::SendMessage(hWnd, WM_MOUSEMOVE, 0, (LPARAM) -1);
	m_bMouse = false;
}

void CWndMain::OnMouseMovePlayer(HWND hWnd)
{
	if (!m_bMouse)
	{
		TRACKMOUSEEVENT tme = { 0 };
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.hwndTrack = hWnd;
		tme.dwHoverTime = 10;
		_TrackMouseEvent(&tme);
		m_bMouse = true;
	}
	m_nLastMoveTime = time(NULL);
}

void CWndMain::OnMouseLButtonUp(HWND hWnd)
{
	if (m_pPlayer && m_pPlayer->IsPlaying())
	{
		m_pPlayer->Pause();
	}
	else if (m_pPlayer && m_pPlayer->IsPausd())
	{
		m_pPlayer->Play();
	}
}

LRESULT WINAPI CWndMain::CallWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWndMain* pWnd = (CWndMain*)GetProp(hWnd, L"MainWndObj");
	HWND hMainWnd = (HWND)GetProp(hWnd, L"MainWnd");
	switch (uMsg)
	{
	case WM_MOUSEHOVER:
		if (IsWindow(hMainWnd) && pWnd)
			pWnd->OnMouseOverPlayer(hWnd);
		break;
	case WM_MOUSELEAVE:
		if (IsWindow(hMainWnd) && pWnd)
			pWnd->OnMouseLeavePlayer(hWnd);
		break;
	case WM_MOUSEMOVE:
		if (IsWindow(hMainWnd) && pWnd)
			pWnd->OnMouseMovePlayer(hWnd);
		break;
	case WM_LBUTTONUP:
		if (IsWindow(hMainWnd) && pWnd)
			pWnd->OnMouseLButtonUp(hWnd);
		break;
	default:
		break;
	}
	if (m_OldWndProc)
		return m_OldWndProc(hWnd, uMsg, wParam, lParam);
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CWndMain::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_TIMER)
	{
		if (wParam == TIMER_ID_POS && m_pPlayer && IsWindowVisible(m_hPlayBarWnd))//
		{
			long nCurPos = m_pPlayer->GetPlayPosition();
			if (nCurPos < 0)
				nCurPos = 0;
			long nDuration = m_pPlayer->GetDuration();
			int nValue = nDuration>0 ? (100 * nCurPos / nDuration) : 0;
			wstring strTime;
			DurationToString(nCurPos, strTime);
			m_pPlayBar->UpdatePlayTime(strTime, nValue);
			return 0;
		}
// 		if (wParam == TIMER_ID_MOUSE)
// 		{
// 			__int64 nNowTime = time(NULL);
// 			if (nNowTime - m_nLastMoveTime > 10 && IsWindow(m_hPlayBarWnd))
// 			{
// 				m_bCatch = true;
// 				::ShowWindow(m_hPlayBarWnd, SW_HIDE);
// 				OutputDebugString(L"隐藏播放控制\n");
// 			}
// 			else
// 			{
// 				OutputDebugString(L"时间间隔不足10秒\n");
// 			}
// 		}
	}
	else if (uMsg == WM_MOVE || uMsg == WM_SIZE)
	{
		UpdatePlayBarPos();
	}
	else if (uMsg == WM_MAIN_MSG_PRO_PLAY)
	{
		bHandled = TRUE;
		return OnMsgSetPlayProgress(wParam, lParam);
	}
	else if (uMsg == WM_MAIN_MSG_PRO_VOLUME)
	{
		bHandled = TRUE;
		return OnMsgSetVolumeProgress(wParam, lParam);
	}
	else if (uMsg == WM_MAIN_MSG_PLAY)
	{
		bHandled = TRUE;
		return OnMsgPlay(wParam, lParam);
	}
	return __super::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

LRESULT CWndMain::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	switch(uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RIGHT)
		{
			if (m_pPlayer)
			{
				long lCurPos = m_pPlayer->GetPlayPosition();
				lCurPos += 5*1000;
				m_pPlayer->SetPlayPosition(lCurPos);
			}
		}
		else if (wParam == VK_LEFT)
		{
			if (m_pPlayer)
			{
				long lCurPos = m_pPlayer->GetPlayPosition();
				lCurPos -= 5*1000;
				if (lCurPos < 1)
					lCurPos = 0;
				m_pPlayer->SetPlayPosition(lCurPos);
			}
		}
		else if (wParam == VK_UP)
		{
			if (m_pPlayer)
			{
				long lCurVolume = m_pPlayer->GetVolume();
				lCurVolume += 5;
				if (lCurVolume > 100)
					lCurVolume = 100;
				m_pPlayer->SetVolume(lCurVolume);
				m_pPlayBar->SetVolume(lCurVolume);
			}
		}
		else if (wParam == VK_DOWN)
		{
			if (m_pPlayer)
			{
				long lCurVolume = m_pPlayer->GetVolume();
				lCurVolume -= 5;
				if (lCurVolume < 0)
					lCurVolume = 0;
				m_pPlayer->SetVolume(lCurVolume);
				m_pPlayBar->SetVolume(lCurVolume);
			}
		}
		else if (wParam == VK_SPACE)
		{
			if (m_pPlayer->IsPlaying())
			{
				m_pPlayer->Pause();
			}
			else if (m_pPlayer->IsPausd())
			{
				m_pPlayer->Play();
			}
		}
		break;

	}
	return 0;
}