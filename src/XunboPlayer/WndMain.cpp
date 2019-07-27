#include "stdafx.h"
#include "WndMain.h"
#include "Resource.h"
#include "Global.h"


CWndMain::CWndMain(const wstring& strUrl, const wstring& strTitle)
: m_hBmpLogo(NULL)
, m_pPlayer(NULL)
, m_pPlayBar(NULL)
, m_hPlayBarWnd(NULL)
, m_strUrl(strUrl)
, m_strTitle(strTitle)
, m_pLblTitle(NULL)
, m_bMouse(false)
, m_bCatch(false)
{
}


CWndMain::~CWndMain()
{
	if (m_hBmpLogo)
	{
		DeleteObject(m_hBmpLogo);
		m_hBmpLogo = NULL;
	}
}

DuiLib::UILIB_RESOURCETYPE CWndMain::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIPRESOURCE;
#endif
}

LPCTSTR CWndMain::GetResourceID() const
{
	return MAKEINTRESOURCE(IDR_DAT1);
}

void CWndMain::OnFinalMessage(HWND hWnd)
{
	KillTimer(m_hWnd, TIMER_ID_POS);
	PostQuitMessage(0);
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CWndMain::InitWindow()
{
	SetIcon(IDI_XUNBOPLAYER);
	m_hBmpLogo = LoadBitmap(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(IDB_BITMAP1));
 	m_pLblTitle = m_PaintManager.FindControl(L"lbl_title");
 	ASSERT(m_pLblTitle);
	m_pLblTitle->SetText(m_strTitle.c_str());
	m_pPlayer = dynamic_cast<CPlayerUI*>(m_PaintManager.FindControl(L"player"));
	if (m_pPlayer && m_hBmpLogo)
	{
		m_pPlayer->SetPlayLogo(m_hBmpLogo);
		m_pPlayer->SetPlayerCallback(this, m_hWnd);
		if (!m_strUrl.empty())
			m_pPlayer->PlayVideo(m_strUrl);
		long nVolume = CGlobal::Instance()->GetVolume();
		m_pPlayer->SetVolume(nVolume);
	}
}

void CWndMain::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(L"btn_open_video") == 0)
	{
		OnOpenVideo();
		return;
	}
	__super::OnClick(msg);
}

void CWndMain::ShowWindow(bool bShow /* = true */, bool bTakeFocus /* = true */)
{
	__super::ShowWindow(bShow, bTakeFocus);
	m_pPlayBar = new CWndPlayBar(m_hWnd);
	m_pPlayBar->Create(m_hWnd, L"", WS_VISIBLE|WS_POPUP|WS_CHILD, 0);
	m_hPlayBarWnd = m_pPlayBar->GetHWND();
	UpdatePlayBarPos();
	if (m_OldWndProc == NULL)
	{
		HWND hWnd = m_pPlayer->GetPlayerWnd();
		HWND hPlayerWnd = ::GetFirstChild(hWnd);
		m_OldWndProc = (WNDPROC)GetWindowLong(hPlayerWnd, GWL_WNDPROC);
		LONG lRet = SetWindowLong(hPlayerWnd, GWL_WNDPROC, (LONG)CallWindowProc);
		SetProp(hPlayerWnd, L"MainWndObj", this);
		SetProp(hPlayerWnd, L"MainWnd", m_hWnd);
	}
}

BOOL CWndMain::OnOpenVideo()
{
	static LPCWSTR lpFilter = L"ÊÓÆµÎÄ¼þ(*.wmv;*.mp4;*.avi;*.flv;*.rmvb;*.mkv)\
							\0*.wmv;*.mp4;*.avi;*.flv;*.rmvb;*.mkv\0\0";
	wstring strBootPath;
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH + 1] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
	ofn.lpstrFilter = lpFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	wchar_t szPath[MAX_PATH + 1] = { 0 }, szOpenFileNames[MAX_PATH + 1] = { 0 };
	if (GetOpenFileName(&ofn))
	{
		if (m_pPlayer)
		{
			m_pPlayer->Stop();
			m_pPlayer->PlayVideo(ofn.lpstrFile);
		}
		return TRUE;
	}
	return FALSE;
}

void CWndMain::UpdatePlayBarPos()
{
	if (m_pPlayBar && IsWindow(m_hPlayBarWnd))
	{
		RECT rcWnd, rcPos;
		GetWindowRect(m_hWnd, &rcWnd);
		int nWidth = rcWnd.right - rcWnd.left - 60;
		int nHeight = 60;
		rcPos.left = rcWnd.left + 30;
		rcPos.top = rcWnd.bottom - 60 - 20;
		rcPos.right = rcPos.left + nWidth;
		rcPos.bottom = rcPos.top + nHeight;
		::SetWindowPos(m_hPlayBarWnd, HWND_TOP, rcPos.left, rcPos.top, nWidth, nHeight, SWP_HIDEWINDOW);
		m_pPlayBar->UpdatePos(nWidth, nHeight);
	}
}

CControlUI* CWndMain::CreateControl(LPCTSTR pstrClass)
{
	if (wcscmp(pstrClass, L"PlayerUI") == 0)
		return new CPlayerUI;
	return NULL;
}

void CWndMain::DurationToString(long nDuration, OUT wstring& strTime)
{
	if (nDuration <= 0)
	{
		strTime.assign(L"00:00");
		return;
	}
	long nSeconds = nDuration / 1000;
	int nHour = nSeconds / 3600;
	nSeconds = nSeconds % 3600;
	int nMin = nSeconds / 60;
	nSeconds = nSeconds % 60;
	wchar_t szTime[32] = { 0 };
	if (nHour > 0)
	{
		swprintf_s(szTime, L"%d:%02d:%02d", nHour, nMin, nSeconds);
	}
	else
	{
		swprintf_s(szTime, L"%02d:%02d", nMin, nSeconds);
	}
	strTime.assign(szTime);
}
WNDPROC CWndMain::m_OldWndProc = NULL;
