#include "stdafx.h"
#include "PlayerUI.h"


CPlayerUI::CPlayerUI()
{
}


CPlayerUI::~CPlayerUI()
{

}

bool CPlayerUI::PlayVideo(const wstring& strVideo)
{
	HRESULT hr = E_FAIL;
	if (m_player.m_pAPlayer)
	{
		Stop();
		hr = m_player.m_pAPlayer->Open(strVideo.c_str());
	}
	return hr == S_OK;
}

void CPlayerUI::Play()
{
	if (m_player.m_pAPlayer)
		m_player.m_pAPlayer->Play();
}

void CPlayerUI::Pause()
{
	if (m_player.m_pAPlayer)
		m_player.m_pAPlayer->Pause();
}

void CPlayerUI::Stop()
{
	if (m_player.m_pAPlayer)
	{
		if (m_player.m_pAPlayer->GetState() == PS_PAUSED
			|| m_player.m_pAPlayer->GetState() == PS_PLAY)
		{
			m_player.m_pAPlayer->Close();
		}
	}
}

void CPlayerUI::SetPlayPosition(long nPos)
{
	if (m_player.m_pAPlayer)
	{
		m_player.m_pAPlayer->SetPosition(nPos);
	}
}

long CPlayerUI::GetPlayPosition()
{
	if (m_player.m_pAPlayer)
	{
		return m_player.m_pAPlayer->GetPosition();
	}
	return 0L;
}

long CPlayerUI::GetVolume()
{
	if (m_player.m_pAPlayer)
	{
		return m_player.m_pAPlayer->GetVolume();
	}
	return 0L;
}

void CPlayerUI::SetVolume(long lVolume)
{
	if (m_player.m_pAPlayer)
	{
		m_player.m_pAPlayer->SetVolume(lVolume);
	}
}

bool CPlayerUI::IsPlaying()
{
	if (m_player.m_pAPlayer)
	{
		return m_player.m_pAPlayer->GetState() == PS_PLAY;
	}
	return false;
}

bool CPlayerUI::IsPausd()
{
	if (m_player.m_pAPlayer)
	{
		return m_player.m_pAPlayer->GetState() == PS_PAUSED;
	}
	return false;
}

bool CPlayerUI::IsSeeking()
{
	if (m_player.m_pAPlayer)
	{
		long lRet = m_player.m_pAPlayer->IsSeeking();
		return m_player.m_pAPlayer->IsSeeking() == 1;
	}
	return false;
}

long CPlayerUI::GetBufferProgress()
{
	if (m_player.m_pAPlayer)
	{
		return m_player.m_pAPlayer->GetBufferProgress();
	}
	return 0;
}

void CPlayerUI::SetPlayLogo(HBITMAP hBmp)
{
	if (m_player.m_pAPlayer)
	{
		m_player.m_pAPlayer->SetCustomLogo(LONG(hBmp));
	}
}

long CPlayerUI::GetDuration()
{
	if (m_player.m_pAPlayer)
	{
		return m_player.m_nDuration;
	}
	return 0L;
}

int CPlayerUI::GetVideoWidth()
{
	if (m_player.m_pAPlayer)
	{
		return m_player.m_pAPlayer->GetVideoWidth();
	}
	return 0;
}

int CPlayerUI::GetVideoHeight()
{
	if (m_player.m_pAPlayer)
	{
		return m_player.m_pAPlayer->GetVideoHeight();
	}
	return 0;
}

HWND CPlayerUI::GetPlayerWnd()
{
	if (m_player.m_pAPlayer)
	{
		return m_player.m_hAPlayerWnd;
	}
	return NULL;
}

void CPlayerUI::SetPlayerCallback(IPlayerCallback* pCallback, void* lpUserParam)
{
	m_player.SetPlayerCallback(pCallback, lpUserParam);
}

void CPlayerUI::DoInit()
{
	m_player.Initialize(m_pManager->GetPaintWindow());
	if (m_player.CreateAPlayerWindow() == TRUE)
	{
		if (!m_strUrl.empty())
			PlayVideo(m_strUrl);
	}
	else
	{
		ASSERT(L"º”‘ÿAPlayer ß∞‹£°");
	}
}

void CPlayerUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (wcscmp(pstrName, L"url") == 0)
	{
		m_strUrl.assign(pstrValue);
	}
	else
		__super::SetAttribute(pstrName, pstrValue);
}

void CPlayerUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	m_player.SetPos(rc);
}

void CPlayerUI::SetInternVisible(bool bVisible /* = true */)
{
	CControlUI::SetInternVisible(bVisible);
	HWND hWnd = m_player.m_hAPlayerWnd;
	if (hWnd)
		::ShowWindow(hWnd, bVisible ? SW_SHOW : SW_HIDE);
}
