#include "stdafx.h"
#include "WndMain.h"
#include "MsgBox\SimpleMsgWnd.h"
#include <shellapi.h>
#include "resource.h"



void CWndMain::OnOpenComplete(void* lpUserParam, long lDuiration)
{
	if (IsWindow(m_hPlayBarWnd))
	{
		wstring strTime;
		DurationToString(lDuiration, strTime);
		m_pPlayBar->UpdateTotalTime(strTime);
	}
//自适应
// 	int nWidth = m_pPlayer->GetVideoWidth();
// 	int nHeight = m_pPlayer->GetVideoHeight();
// 	int nX = GetSystemMetrics(SM_CXSCREEN);
// 	int nY = GetSystemMetrics(SM_CYSCREEN);
// 	int nDestWidth = 0, nDestHeight = 0;
// 	if (nWidth < nX && nHeight < nY)
// 	{
// 		nDestWidth = nWidth;
// 		nDestHeight = nHeight;
// 	}
// 	else if (nWidth < nX)
// 	{
// 		nDestHeight = nY;
// 		nDestWidth = nWidth*nY/nHeight;
// 	}
// 	else
// 	{
// 		nDestWidth = nX;
// 		nDestHeight = nHeight*nX / nWidth;
// 	}
// 	int xPos = (nX - nDestWidth) / 2;
// 	int yPos = (nY - nDestHeight) / 2;
// 	::SetWindowPos(m_hWnd, HWND_TOP, xPos, yPos, nDestWidth, nDestHeight, SWP_SHOWWINDOW);
	SetTimer(m_hWnd, TIMER_ID_POS, 1000, NULL);
}

void CWndMain::OnStateChange(void* lpUserParam, PlayerState state)
{
	if (IsWindow(m_hPlayBarWnd))
	{
		m_pPlayBar->UpdateState(state);
	}
	if (state == Ps_Finish || state == Ps_Error)
	{
		KillTimer(m_hWnd, TIMER_ID_POS);
	}
}

void CWndMain::OnVolumeChange(void* lpUserParam, long nVolume)
{
	if (m_pPlayBar)
		m_pPlayBar->SetVolume(nVolume);
}

void CWndMain::OnDownloadCodec(void* lpUserParam, char* pCodecPath)
{
//http://aplayer.open.xunlei.com/codecs.zip
	CSimpleMsgWnd* pWnd = new CSimpleMsgWnd;
	pWnd->SetMsg(L"缺少解码库，无法播放该视频！");
	pWnd->Create(m_hWnd, L"下载解码库", UI_WNDSTYLE_DIALOG, 0);
	pWnd->CenterWindow();
	pWnd->ShowModal();
}

LRESULT CWndMain::OnMsg(void* lpUserParam, HWND hWnd, LONG nMessage, LONG wParam, LONG lParam)
{
	switch (nMessage)
	{
	case WM_RBUTTONUP:
	{
		HMENU hMenu = ::LoadMenu(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(IDR_MENU1));
		if(hMenu)
		{
			HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
			::SetForegroundWindow(hWnd);
			POINT pt;
			::GetCursorPos(&pt);
			if (m_pPlayer)
			{
				if (m_pPlayer->IsPlaying())
					::ModifyMenu(hSubMenu, 1, MF_BYPOSITION | MF_STRING, ID_MAINMENU_PLAY, L"暂停");
				else if (m_pPlayer->IsPausd())
					::ModifyMenu(hSubMenu, 1, MF_BYPOSITION | MF_STRING, ID_MAINMENU_PLAY, L"播放");
				else
					::ModifyMenu(hSubMenu, 1, MF_BYPOSITION | MF_GRAYED, ID_MAINMENU_PLAY, L"播放");
			}
			::TrackPopupMenu(hSubMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hWnd, NULL);
			::DestroyMenu(hMenu);
		}
	}
		break;
	default:
		break;
	}
	return 0;
}

void CWndMain::OnSeekCompleted(void* lpUserParam, long nPosition)
{

}

void CWndMain::OnBuffer(void* lpUserParam, long nPercent)
{
	if (IsWindow(m_hPlayBarWnd))
	{
		m_pPlayBar->UpdateState(Ps_Loading, nPercent);
	}
}

LRESULT CWndMain::OnMenuClick(WORD wId, BOOL& bHandled)
{
	switch (wId)
	{
	case ID_MAINMENU_OPEN://打开文件
		OnOpenVideo();
		break;
	case ID_MAINMENU_PLAY://播放、暂停
		if (m_pPlayer)
		{
			if (m_pPlayer->IsPlaying())
				m_pPlayer->Pause();
			else
				m_pPlayer->Play();
		}
		break;
	case ID_MAINMENU_EXIT://退出
		Close();
		break;
	default:
		break;
	}
	bHandled = TRUE;
	return 0;
}

