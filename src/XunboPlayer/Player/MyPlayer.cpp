#include "stdafx.h"
#include "MyPlayer.h"
#include <commdlg.h>
#include <atlhost.h>

#define EventId_Message				1
#define EventId_StateChanged		2
#define EventId_OpenSuccess			3
#define EventId_SeekCompleted		4
#define EventId_OnBuffer			5
#define EventId_OnVideoSizeChanged	6
#define EventId_OnDownloadCodec		7
#define EventId_OnEvent				8
#define CONFIGID_PLAYRESULT		7		// Play result


CMyPlayer::CMyPlayer()
: m_hFullScreenWnd(NULL)
, m_pCallback(NULL)
, m_lpUserParam(NULL)
{
	memset(&m_rcPos, 0, sizeof(RECT));
}


CMyPlayer::~CMyPlayer()
{
	Close();
}

void CMyPlayer::SetPlayerCallback(IPlayerCallback* pCallback, void* lpParam)
{
	m_pCallback = pCallback;
	m_lpUserParam = lpParam;
}

void CMyPlayer::DetachCallback()
{
	m_pCallback = NULL;
	m_lpUserParam = NULL;
}

HRESULT CMyPlayer::OnMessage(LONG nMessage, LONG wParam, LONG lParam)
{
	switch (nMessage)
	{
		case WM_LBUTTONDBLCLK:
			if (::IsWindowVisible(m_hFullScreenWnd))
				ExitFullScreen();
			else
				EnterFullScreen();
			break;
		case WM_KEYDOWN:
			switch (LOWORD(wParam))
			{
			case VK_ESCAPE:
				ExitFullScreen();
				break;
			case VK_SPACE:
				if (m_pAPlayer)
				{
					if (m_pAPlayer->GetState() == PS_PAUSED)
						m_pAPlayer->Play();
					else if (m_pAPlayer->GetState() == PS_PLAY)
						m_pAPlayer->Pause();
				}
				break;
			case VK_RIGHT:
			{
				if (m_pAPlayer)
				{
					long lCurPos = m_pAPlayer->GetPosition();
					lCurPos += 5 * 1000;
					m_pAPlayer->SetPosition(lCurPos);
				}
				break;
			}
			case VK_LEFT:
			{
				if (m_pAPlayer)
				{
					long lCurPos = m_pAPlayer->GetPosition();
					lCurPos -= 5 * 1000;
					if (lCurPos < 1)
						lCurPos = 0;
					m_pAPlayer->SetPosition(lCurPos);
				}
				break;
			}
			case VK_UP:
			{
				if (m_pAPlayer)
				{
					long lCurVolume = m_pAPlayer->GetVolume();
					lCurVolume += 5;
					if (lCurVolume > 100)
						lCurVolume = 100;
					m_pAPlayer->SetVolume(lCurVolume);
					if (m_pCallback)
						m_pCallback->OnVolumeChange(m_lpUserParam, lCurVolume);
				}
				break;
			}
			case VK_DOWN:
			{
				if (m_pAPlayer)
				{
					long lCurVolume = m_pAPlayer->GetVolume();
					lCurVolume -= 5;
					if (lCurVolume < 0)
						lCurVolume = 0;
					m_pAPlayer->SetVolume(lCurVolume);
					if (m_pCallback)
						m_pCallback->OnVolumeChange(m_lpUserParam, lCurVolume);
				}
				break;
			}
			}
			break;
	}
	if (m_pCallback)
		m_pCallback->OnMsg(m_lpUserParam, m_hAPlayerWnd, nMessage, wParam, lParam);
	return S_OK;
}

int CMyPlayer::GetConfigInt(int nConfigId)
{
	_bstr_t strConfig = m_pAPlayer->GetConfig(nConfigId);
	wstring str(strConfig.GetBSTR());
	int nResult = _wtoi(str.c_str());
	return nResult;
}

HRESULT CMyPlayer::OnStateChanged(LONG nOldState, LONG nNewState)
{
	switch (nNewState)
	{
	case PS_READY:
	{
		DWORD dwPlayResult = GetConfigInt(CONFIGID_PLAYRESULT);
		switch (dwPlayResult)
		{
		case S_OK:
			if (m_pCallback)
				m_pCallback->OnStateChange(m_lpUserParam, Ps_Finish);
			break;

		case S_FALSE:
			if (m_pCallback)
				m_pCallback->OnStateChange(m_lpUserParam, Ps_Begin);
			break;

		default:
			if (m_pCallback)
				m_pCallback->OnStateChange(m_lpUserParam, Ps_Error);
			break;
		}
	}
		break;

	case PS_OPENING:
		if (m_pCallback)
			m_pCallback->OnStateChange(m_lpUserParam, Ps_Opening);
		break;

	case PS_PAUSING:
		if (m_pCallback)
			m_pCallback->OnStateChange(m_lpUserParam, Ps_Pausing);
		break;

	case PS_PAUSED:
		if (m_pCallback)
			m_pCallback->OnStateChange(m_lpUserParam, Ps_Paused);
		break;

	case PS_PLAYING:
		if (m_pCallback)
			m_pCallback->OnStateChange(m_lpUserParam, Ps_Playing);
		break;

	case PS_PLAY:
		if (m_pCallback)
			m_pCallback->OnStateChange(m_lpUserParam, Ps_Play);
		break;

	case PS_CLOSING:
		if (m_pCallback)
			m_pCallback->OnStateChange(m_lpUserParam, Ps_Closing);
		break;
	}

	return S_OK;
}

HRESULT CMyPlayer::OnSeekCompleted(LONG nPosition)
{
	if (m_pCallback)
		m_pCallback->OnSeekCompleted(m_lpUserParam, nPosition);
	return S_OK;
}

HRESULT CMyPlayer::OnBuffer(LONG nPercent)
{
	if (m_pCallback)
		m_pCallback->OnBuffer(m_lpUserParam, nPercent);
	return S_OK;
}

HRESULT CMyPlayer::OnVideoSizeChanged(void)
{
	return S_OK;
}

void CMyPlayer::Initialize(HWND hWndParent)
{
	m_nPosition = 0;
	m_nDuration = 1000;
	m_pAPlayer = NULL;
	m_pConnectionPoint = NULL;
	m_dwCookie = 0;
	m_hAPlayerWnd = NULL;
	m_hModule = NULL;
	m_hParentWnd = hWndParent;
}

BOOL CMyPlayer::GetPathFromFullName(const TCHAR * pcszFullName, TCHAR * pszPath)
{
	int nLength = lstrlen(pcszFullName);
	if (nLength >= MAX_PATH)
		return FALSE;
	lstrcpy(pszPath, pcszFullName);
	TCHAR * pEnd = pszPath + nLength - 1;
	while (pEnd > pszPath && *pEnd != '\\') pEnd--;
	*pEnd = '\0';
	return TRUE;
}

void CMyPlayer::Close()
{
	if (m_hFullScreenWnd)
	{
		CloseWindow(m_hFullScreenWnd);
	}
	if (m_pAPlayer != NULL)
	{
		m_pAPlayer->Close();

		if (m_dwCookie != 0)
		{
			m_pConnectionPoint->Unadvise(m_dwCookie);
			m_dwCookie = 0;
		}

		m_pAPlayer->Release();
		m_pAPlayer = NULL;

		if (IsWindow(m_hAPlayerWnd))
		{
			DestroyWindow(m_hAPlayerWnd);
			m_hAPlayerWnd = NULL;
		}

		FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
}

HRESULT CMyPlayer::CreateInstanceFromFile(const TCHAR * pcszPath, REFCLSID rclsid, REFIID riid, IUnknown * pUnkOuter, LPVOID * ppv)
{
	m_hModule = ::LoadLibraryEx(pcszPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (m_hModule == NULL)
		return TYPE_E_CANTLOADLIBRARY;

	typedef HRESULT(STDAPICALLTYPE *_pfnDllGetClassObject)(REFCLSID, REFIID, LPVOID*);
	_pfnDllGetClassObject pfnDllGetClassObject = (_pfnDllGetClassObject)GetProcAddress(m_hModule, "DllGetClassObject");
	if (pfnDllGetClassObject == NULL)
		return CO_E_ERRORINDLL;

	CComPtr<IClassFactory> spCF;
	HRESULT hr = (*pfnDllGetClassObject)(rclsid, IID_IClassFactory, (LPVOID*)&spCF);
	if (FAILED(hr))
		return hr;

	hr = spCF->CreateInstance(pUnkOuter, riid, ppv);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

BOOL CMyPlayer::CreateAPlayerWindow()
{
	TCHAR szFullName[MAX_PATH];
	GetModuleFileNameW(NULL, szFullName, MAX_PATH);
	TCHAR szPath[MAX_PATH];
	GetPathFromFullName(szFullName, szPath);
	wsprintf(szPath, _T("%s\\APlayer.dll"), szPath);
	HRESULT hr = CreateInstanceFromFile(szPath,
		__uuidof(APlayer3Lib::Player),
		__uuidof(APlayer3Lib::IPlayer),
		NULL,
		(void **)&m_pAPlayer);

	if (SUCCEEDED(hr))
	{
		m_hAPlayerWnd = ::CreateWindowEx(WS_EX_TOOLWINDOW, _T(ATLAXWIN_CLASS), _T(""),
			WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
			m_hParentWnd, NULL, NULL, NULL);
	}

	if (::IsWindow(m_hAPlayerWnd))
	{
		hr = AtlAxAttachControl(m_pAPlayer, m_hAPlayerWnd, NULL);
	}

	CComQIPtr<IConnectionPointContainer> spContainer;
	if (SUCCEEDED(hr))
	{
		hr = m_pAPlayer->QueryInterface(__uuidof(IConnectionPointContainer),
			(void **)&spContainer);
	}

	if (SUCCEEDED(hr))
	{
		hr = spContainer->FindConnectionPoint(__uuidof(APlayer3Lib::_IPlayerEvents),
			&m_pConnectionPoint);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pConnectionPoint->Advise(this, &m_dwCookie);
	}

	if (SUCCEEDED(hr))
	{
		m_pAPlayer->SetCustomLogo(-1);
		return TRUE;
	}
	else
		return FALSE;
}

STDMETHODIMP CMyPlayer::QueryInterface(REFIID iid, void ** ppvObject)
{
	*ppvObject = this;
	return S_OK;
}

ULONG __stdcall CMyPlayer::AddRef(void)
{
	return 1;
}

ULONG __stdcall CMyPlayer::Release(void)
{
	return 0;
}

STDMETHODIMP CMyPlayer::GetTypeInfoCount(unsigned int FAR* pctinfo)
{
	*pctinfo = 0;
	return E_NOTIMPL;
}

STDMETHODIMP CMyPlayer::GetTypeInfo(unsigned int iTInfo, LCID lcid, ITypeInfo FAR* FAR* ppTInfo)
{
	*ppTInfo = NULL;
	return E_NOTIMPL;
}

STDMETHODIMP CMyPlayer::GetIDsOfNames(REFIID riid,
	OLECHAR FAR* FAR* rgszNames,
	unsigned int cNames,
	LCID lcid,
	DISPID FAR* rgDispId)
{
	return E_NOTIMPL;
}

STDMETHODIMP CMyPlayer::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR* pDispParams, VARIANT FAR* pVarResult, EXCEPINFO FAR* pExcepInfo, unsigned int FAR* puArgErr)
{
	switch (dispIdMember)
	{
		case EventId_Message:
		{
			if (pDispParams->cArgs == 3)
			{
				OnMessage(pDispParams->rgvarg[2].lVal,
					pDispParams->rgvarg[1].lVal,
					pDispParams->rgvarg[0].lVal);
			}
		}
		break;

		case EventId_StateChanged:
		{
			if (pDispParams->cArgs == 2)
			{
				OnStateChanged(pDispParams->rgvarg[1].lVal,
					pDispParams->rgvarg[0].lVal);
			}
		}
		break;

		case EventId_OpenSuccess:
		{
			if (pDispParams->cArgs == 0)
			{
				m_nDuration = m_pAPlayer->GetDuration();
				if (m_pCallback)
					m_pCallback->OnOpenComplete(m_lpUserParam, m_nDuration);
			}
		}
		break;

		case EventId_SeekCompleted:
		{
			if (pDispParams->cArgs == 1)
			{
				OnSeekCompleted(pDispParams->rgvarg[0].lVal);
			}
		}
		break;

		case EventId_OnBuffer:
		{
			if (pDispParams->cArgs == 1)
			{
				OnBuffer(pDispParams->rgvarg[0].lVal);
			}
		}
		break;

		case EventId_OnVideoSizeChanged:
		{
			if (pDispParams->cArgs == 0)
			{
				OnVideoSizeChanged();
			}
		}
		break;

		case EventId_OnDownloadCodec:
		{
			if (pDispParams->cArgs == 1)
			{
				if (m_pCallback)
				{
					_bstr_t b = pDispParams->rgvarg[0].bstrVal;
					m_pCallback->OnDownloadCodec(m_lpUserParam, b);
				}
				m_pAPlayer->SetConfig(19, _bstr_t("0"));
			}
		}
		break;

		case EventId_OnEvent:
		{
			if (pDispParams->cArgs == 2)
			{

			}
		}
		break;
	}

	return S_OK;
}

void CMyPlayer::EnterFullScreen(void)
{
	if (m_hFullScreenWnd == NULL)
	{
		// Register window class
		WNDCLASSEX wc;
		wc.cbSize = sizeof(wc);
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
		wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = NULL;
		wc.hIconSm = NULL;
		wc.hInstance = NULL;
		wc.lpfnWndProc = FullScreenWindowProc;
		wc.lpszClassName = _T("FullScreenContainer");
		wc.lpszMenuName = 0;
		wc.style = CS_DBLCLKS;

		if (!::RegisterClassEx(&wc))
		{
			int nError = GetLastError();
			if (nError != ERROR_CLASS_ALREADY_EXISTS)
				return;
		}
		int w = ::GetSystemMetrics(SM_CXSCREEN);
		int h = ::GetSystemMetrics(SM_CYSCREEN);
		m_hFullScreenWnd = ::CreateWindowEx(WS_EX_TOPMOST, wc.lpszClassName, L"", WS_POPUP, 0, 0, w, h, NULL, NULL, NULL, NULL);
		::SetProp(m_hFullScreenWnd, L"this", (HANDLE)this);
	}

	::ShowWindow(m_hFullScreenWnd, SW_SHOW);
	::SetParent(m_hAPlayerWnd, m_hFullScreenWnd);
	::ShowWindow(m_hParentWnd, SW_HIDE);
	// Set Position
	RECT rc;
	::GetClientRect(m_hFullScreenWnd, &rc);
	::SetWindowPos(m_hAPlayerWnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 0);
}


void CMyPlayer::ExitFullScreen(void)
{
	::SetParent(m_hAPlayerWnd, m_hParentWnd);
	::SetWindowPos(m_hAPlayerWnd, HWND_TOP, m_rcPos.left, m_rcPos.top, m_rcPos.right - m_rcPos.left, m_rcPos.bottom - m_rcPos.top, SWP_SHOWWINDOW);
	::ShowWindow(m_hFullScreenWnd, SW_HIDE);
	::ShowWindow(m_hParentWnd, SW_SHOW);
}

LONG WINAPI CMyPlayer::FullScreenWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CMyPlayer* pPlayer = (CMyPlayer*)GetProp(hWnd, L"this");
	switch (uMsg)
	{
	case WM_CLOSE:
		if (pPlayer)
			pPlayer->ExitFullScreen();
		return 0;

	case WM_LBUTTONDBLCLK:
		if (pPlayer)
			pPlayer->ExitFullScreen();
		break;

	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			if (pPlayer)
				pPlayer->ExitFullScreen();
			break;
		case VK_SPACE:
			if (pPlayer)
			{
				if (pPlayer->m_pAPlayer->GetState() == PS_PAUSED)
					pPlayer->m_pAPlayer->Play();
				else if (pPlayer->m_pAPlayer->GetState() == PS_PLAY)
					pPlayer->m_pAPlayer->Pause();
			}
			break;
		case VK_RIGHT:
			{
				if (pPlayer)
				{
					long lCurPos = pPlayer->m_pAPlayer->GetPosition();
					lCurPos += 5 * 1000;
					pPlayer->m_pAPlayer->SetPosition(lCurPos);
				}
				break;
			}
		case VK_LEFT:
			{
				if (pPlayer)
				{
					long lCurPos = pPlayer->m_pAPlayer->GetPosition();
					lCurPos -= 5 * 1000;
					if (lCurPos < 1)
						lCurPos = 0;
					pPlayer->m_pAPlayer->SetPosition(lCurPos);
				}
				break;
			}
		case VK_UP:
			{
				if (pPlayer)
				{
					long lCurVolume = pPlayer->m_pAPlayer->GetVolume();
					lCurVolume += 5;
					if (lCurVolume > 100)
						lCurVolume = 100;
					pPlayer->m_pAPlayer->SetVolume(lCurVolume);
					if (pPlayer->m_pCallback)
						pPlayer->m_pCallback->OnVolumeChange(pPlayer->m_lpUserParam, lCurVolume);
				}
				break;
			}
		case VK_DOWN:
			{
				if (pPlayer)
				{
					long lCurVolume = pPlayer->m_pAPlayer->GetVolume();
					lCurVolume -= 5;
					if (lCurVolume < 0)
						lCurVolume = 0;
					pPlayer->m_pAPlayer->SetVolume(lCurVolume);
					if (pPlayer->m_pCallback)
						pPlayer->m_pCallback->OnVolumeChange(pPlayer->m_lpUserParam, lCurVolume);
				}
				break;
			}
		}
		break;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void CMyPlayer::SetPos(const RECT& rc)
{
	if (m_hAPlayerWnd && IsWindow(m_hAPlayerWnd))
	{
		::SetWindowPos(m_hAPlayerWnd, HWND_TOP, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
	}
	m_rcPos = rc;
}
