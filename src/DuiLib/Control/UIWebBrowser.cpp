#include "StdAfx.h"
#include "UIWebBrowser.h"
#include <atlconv.h>
#include <atlcomcli.h>
#include "../Utils/downloadmgr.h"
#include <mshtml.h>
#include <shlguid.h>
#pragma comment(lib, "atlsd")



DuiLib::CWebBrowserUI::CWebBrowserUI()
: m_pWebBrowser2(NULL)
, m_pHtmlWnd2(NULL)
, m_pWebBrowserEventHandler(NULL)
, m_bAutoNavi(false)
, m_dwRef(0)
, m_dwCookie(0)
, m_bSendMeg(false)
{
	m_clsid = CLSID_WebBrowser;
}

bool DuiLib::CWebBrowserUI::DoCreateControl()
{
	if (!CActiveXUI::DoCreateControl())
		return false;
	GetManager()->AddTranslateAccelerator(this);
	GetControl(IID_IWebBrowser2,(LPVOID*)&m_pWebBrowser2);
	RegisterEventHandler(TRUE);
	if ( m_bAutoNavi && !m_sHomePage.IsEmpty())
	{
		this->Navigate2(m_sHomePage);
	}

	if(m_pWebBrowser2) m_pWebBrowser2->put_RegisterAsBrowser(VARIANT_TRUE);
	CComPtr<IOleObject> pOleObj = NULL;
	m_pWebBrowser2->QueryInterface( IID_IOleObject ,(void**)&pOleObj );
	if( pOleObj )
		pOleObj->SetClientSite(this);
	return true;
}

void DuiLib::CWebBrowserUI::ReleaseControl()
{
	m_bCreated=false;
	GetManager()->RemoveTranslateAccelerator(this);
	RegisterEventHandler(FALSE);
	CActiveXUI::ReleaseControl();
}

DuiLib::CWebBrowserUI::~CWebBrowserUI()
{
	ReleaseControl();
}

STDMETHODIMP DuiLib::CWebBrowserUI::GetTypeInfoCount( UINT *iTInfo )
{
	*iTInfo = 0;
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo )
{
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::GetIDsOfNames( REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid,DISPID *rgDispId )
{
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid,WORD wFlags, DISPPARAMS* pDispParams,VARIANT* pVarResult, EXCEPINFO* pExcepInfo,UINT* puArgErr )
{
	if ((riid != IID_NULL))
		return E_INVALIDARG;

	switch(dispIdMember)
	{
	case DISPID_BEFORENAVIGATE2:
		BeforeNavigate2(
			pDispParams->rgvarg[6].pdispVal,
			pDispParams->rgvarg[5].pvarVal,
			pDispParams->rgvarg[4].pvarVal,
			pDispParams->rgvarg[3].pvarVal,
			pDispParams->rgvarg[2].pvarVal,
			pDispParams->rgvarg[1].pvarVal,
			pDispParams->rgvarg[0].pboolVal);
		break;
	case DISPID_COMMANDSTATECHANGE:
		CommandStateChange(
			pDispParams->rgvarg[1].lVal,
			pDispParams->rgvarg[0].boolVal);
		break;
	case DISPID_NAVIGATECOMPLETE2:
		NavigateComplete2(
			pDispParams->rgvarg[1].pdispVal,
			pDispParams->rgvarg[0].pvarVal);
		break;
	case DISPID_NAVIGATEERROR:
		NavigateError(
			pDispParams->rgvarg[4].pdispVal,
			pDispParams->rgvarg[3].pvarVal,
			pDispParams->rgvarg[2].pvarVal,
			pDispParams->rgvarg[1].pvarVal,
			pDispParams->rgvarg[0].pboolVal);
		break;
	case DISPID_STATUSTEXTCHANGE:
		break;
		//  	case DISPID_NEWWINDOW2:
		//  		break;
	case DISPID_NEWWINDOW3:
		NewWindow3(
			pDispParams->rgvarg[4].ppdispVal,
			pDispParams->rgvarg[3].pboolVal,
			pDispParams->rgvarg[2].uintVal,
			pDispParams->rgvarg[1].bstrVal,
			pDispParams->rgvarg[0].bstrVal);
		break;
	case DISPID_DOCUMENTCOMPLETE:
		DocComplete(pDispParams->rgvarg[1].pdispVal,
				pDispParams->rgvarg[0].pvarVal);
		if(m_pWebBrowser2&&m_bSendMeg){
			::PostMessage(m_hParentWnd, WM_USER+601, (WPARAM)this, 0);
			m_bSendMeg = false;
		}
		break;
	case DISPID_DOWNLOADCOMPLETE:
		break;
// 	case DISPID_PROPERTYCHANGE:
// 		if (pDispParams->cArgs>0 && pDispParams->rgvarg[0].vt == VT_BSTR) {
// 			TRACE(_T("PropertyChange(%s)\n"), pDispParams->rgvarg[0].bstrVal);
// 		}
// 		break;
	default:
		return DISP_E_MEMBERNOTFOUND;
	}
	return S_OK;
}

STDMETHODIMP DuiLib::CWebBrowserUI::QueryInterface( REFIID riid, LPVOID *ppvObject )
{
	*ppvObject = NULL;

	if( riid == IID_IDocHostUIHandler)
		*ppvObject = static_cast<IDocHostUIHandler*>(this);
	else if( riid == IID_IDispatch)
		*ppvObject = static_cast<IDispatch*>(this);
	else if( riid == IID_IServiceProvider)
		*ppvObject = static_cast<IServiceProvider*>(this);
	else if (riid == IID_IOleCommandTarget)
		*ppvObject = static_cast<IOleCommandTarget*>(this);

	if( *ppvObject != NULL )
		AddRef();
	return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

STDMETHODIMP_(ULONG) DuiLib::CWebBrowserUI::AddRef()
{
	InterlockedIncrement(&m_dwRef); 
	return m_dwRef;
}

STDMETHODIMP_(ULONG) DuiLib::CWebBrowserUI::Release()
{
	ULONG ulRefCount = InterlockedDecrement(&m_dwRef);
	return ulRefCount; 
}

void DuiLib::CWebBrowserUI::Navigate2(LPCTSTR lpszUrl, HWND hWnd/*=NULL*/)
{
	if(hWnd) m_hParentWnd = hWnd;
	if (lpszUrl == NULL)
		return;

	if (m_pWebBrowser2)
	{
		::PostMessage(m_hParentWnd, WM_USER+600, (WPARAM)this, 0);
		m_bSendMeg = true;
		HRESULT hr = m_pWebBrowser2->Navigate(T2BSTR(lpszUrl), NULL, NULL, NULL, NULL);
		
	}
}

void DuiLib::CWebBrowserUI::Refresh()
{
	if (m_pWebBrowser2)
	{
		if(!m_szReloadUrl.IsEmpty()) Navigate2(m_szReloadUrl.GetData(), m_hParentWnd);
		else
			m_pWebBrowser2->Refresh();
	}
}
void DuiLib::CWebBrowserUI::GoBack()
{
	if (m_pWebBrowser2)
	{
		m_pWebBrowser2->GoBack();
	}
}
void DuiLib::CWebBrowserUI::GoForward()
{
	if (m_pWebBrowser2)
	{
		m_pWebBrowser2->GoForward();
	}
}
/// DWebBrowserEvents2
void DuiLib::CWebBrowserUI::BeforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel )
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->BeforeNavigate2(pDisp,url,Flags,TargetFrameName,PostData,Headers,Cancel);
	}
}

void DuiLib::CWebBrowserUI::NavigateError( IDispatch *pDisp,VARIANT * &url,VARIANT *&TargetFrameName,VARIANT *&StatusCode,VARIANT_BOOL *&Cancel )
{
	CDuiString szUrl = _T("");
	if (m_pWebBrowserEventHandler)
	{
		if(m_szReloadUrl.IsEmpty()) m_szReloadUrl = url->bstrVal;
		szUrl = m_pWebBrowserEventHandler->NavigateError(pDisp,url,TargetFrameName,StatusCode,Cancel);
	}
	if(m_szErrPageUrl.IsEmpty()) m_szErrPageUrl = szUrl;
	if(!m_szErrPageUrl.IsEmpty()) Navigate2(m_szErrPageUrl.GetData(), m_hParentWnd);
}

void DuiLib::CWebBrowserUI::NavigateComplete2( IDispatch *pDisp,VARIANT *&url )
{
	CComPtr<IDispatch> spDoc;   
	m_pWebBrowser2->get_Document(&spDoc);   

	if (spDoc)
	{   
		CComQIPtr<ICustomDoc, &IID_ICustomDoc> spCustomDoc(spDoc);   
		if (spCustomDoc)   
			spCustomDoc->SetUIHandler(this);   
	}

	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->NavigateComplete2(pDisp,url);
	}
}

void DuiLib::CWebBrowserUI::ProgressChange( LONG nProgress, LONG nProgressMax )
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->ProgressChange(nProgress,nProgressMax);
	}
}

void DuiLib::CWebBrowserUI::NewWindow3( IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl )
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->NewWindow3(pDisp,Cancel,dwFlags,bstrUrlContext,bstrUrl);
	}
// 	if ((	dwFlags == 0x00040004
// 		||	dwFlags == 0x00000004
// 		||	dwFlags == 0x00000005
// 		||	dwFlags == 0x00000026)
// 		)
// 	{
// 		if ( *pDisp )
// 		{
// 			(*pDisp)->Release();
// 			*pDisp = NULL;
// 		}
// 		*Cancel = VARIANT_FALSE;
// 		return;
// 	}
// 	CDuiString szUrl = bstrUrl;
// 	//GotoUrl(szUrl.GetData());
// 	//ShellExecute(NULL,L"open", L"iexplore.exe", szUrl, NULL, SW_MAXIMIZE);
// 	::PostMessage(m_hParentWnd,WM_USER+700,0,0);
// 	*Cancel = TRUE;
	return;
}
void DuiLib::CWebBrowserUI::CommandStateChange(long Command,VARIANT_BOOL Enable)
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->CommandStateChange(Command,Enable);
	}
}

void DuiLib::CWebBrowserUI::DocComplete(IDispatch *pDisp, VARIANT* sURL)
{
	if(m_pWebBrowserEventHandler){
		m_pWebBrowserEventHandler->DocComplete(pDisp,sURL);
	}
}

// IDownloadManager
STDMETHODIMP DuiLib::CWebBrowserUI::Download( /* [in] */ IMoniker *pmk, /* [in] */ IBindCtx *pbc, /* [in] */ DWORD dwBindVerb, /* [in] */ LONG grfBINDF, /* [in] */ BINDINFO *pBindInfo, /* [in] */ LPCOLESTR pszHeaders, /* [in] */ LPCOLESTR pszRedir, /* [in] */ UINT uiCP )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->Download(pmk,pbc,dwBindVerb,grfBINDF,pBindInfo,pszHeaders,pszRedir,uiCP);
	}
	return S_OK;
}

// IDocHostUIHandler
STDMETHODIMP DuiLib::CWebBrowserUI::ShowContextMenu( DWORD dwID, POINT* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->ShowContextMenu(dwID,pptPosition,pCommandTarget,pDispatchObjectHit);
	}
	return S_OK;
}

STDMETHODIMP DuiLib::CWebBrowserUI::GetHostInfo( DOCHOSTUIINFO* pInfo )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->GetHostInfo(pInfo);
	}
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::ShowUI( DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->ShowUI(dwID,pActiveObject,pCommandTarget,pFrame,pDoc);
	}
	return S_OK;
}

STDMETHODIMP DuiLib::CWebBrowserUI::HideUI()
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->HideUI();
	}
	return S_OK;
}

STDMETHODIMP DuiLib::CWebBrowserUI::UpdateUI()
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->UpdateUI();
	}
	return S_OK;
}

STDMETHODIMP DuiLib::CWebBrowserUI::EnableModeless( BOOL fEnable )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->EnableModeless(fEnable);
	}
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::OnDocWindowActivate( BOOL fActivate )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->OnDocWindowActivate(fActivate);
	}
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::OnFrameWindowActivate( BOOL fActivate )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->OnFrameWindowActivate(fActivate);
	}
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::ResizeBorder( LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow, BOOL fFrameWindow )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->ResizeBorder(prcBorder,pUIWindow,fFrameWindow);
	}
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::TranslateAccelerator( LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->TranslateAccelerator(lpMsg,pguidCmdGroup,nCmdID);
	}
	return S_FALSE;
}

LRESULT DuiLib::CWebBrowserUI::TranslateAccelerator( MSG *pMsg )
{
    if(pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST)
        return S_FALSE;

	if( m_pWebBrowser2 == NULL )
        return E_NOTIMPL;

    // ��ǰWeb���ڲ��ǽ���,��������ټ�
    BOOL bIsChild = FALSE;
    HWND hTempWnd = NULL;
    HWND hWndFocus = ::GetFocus();

    hTempWnd = hWndFocus;
    while(hTempWnd != NULL)
    {
        if(hTempWnd == m_hwndHost)
        {
            bIsChild = TRUE;
            break;
        }
        hTempWnd = ::GetParent(hTempWnd);
    }
    if(!bIsChild)
        return S_FALSE;

	if(pMsg->message == WM_KEYDOWN){
		if(pMsg->wParam == VK_F5){
			//Refresh();
			::PostMessage(m_hParentWnd, WM_USER + 602, 0, 0);
		}
	}

	IOleInPlaceActiveObject *pObj;
	if (FAILED(m_pWebBrowser2->QueryInterface(IID_IOleInPlaceActiveObject, (LPVOID *)&pObj)))
		return S_FALSE;

    HRESULT hResult = pObj->TranslateAccelerator(pMsg);
    pObj->Release();
    return hResult;
}

STDMETHODIMP DuiLib::CWebBrowserUI::GetOptionKeyPath( LPOLESTR* pchKey, DWORD dwReserved )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->GetOptionKeyPath(pchKey,dwReserved);
	}
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::GetDropTarget( IDropTarget* pDropTarget, IDropTarget** ppDropTarget )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->GetDropTarget(pDropTarget,ppDropTarget);
	}
	return S_FALSE;	// ʹ��ϵͳ��ק
}

STDMETHODIMP DuiLib::CWebBrowserUI::GetExternal( IDispatch** ppDispatch )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->GetExternal(ppDispatch);
	}
	return S_FALSE;
}

STDMETHODIMP DuiLib::CWebBrowserUI::TranslateUrl( DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->TranslateUrl(dwTranslate,pchURLIn,ppchURLOut);
	}
    else
    {
        *ppchURLOut = 0;
        return E_NOTIMPL;
    }
}

STDMETHODIMP DuiLib::CWebBrowserUI::FilterDataObject( IDataObject* pDO, IDataObject** ppDORet )
{
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->FilterDataObject(pDO,ppDORet);
	}
    else
    {
        *ppDORet = 0;
        return E_NOTIMPL;
    }
}

STDMETHODIMP DuiLib::CWebBrowserUI::SaveObject() 
{  
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk) 
{
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::OnShowWindow(BOOL fShow)
{
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::RequestNewObjectLayout()
{
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::GetContainer(LPOLECONTAINER FAR* ppContainer)
{
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::ShowObject()
{
	return S_OK;
}


STDMETHODIMP DuiLib::CWebBrowserUI::ShowHelp(HWND hwnd,LPOLESTR pszHelpFile,UINT uCommand,DWORD dwData,POINT ptMouse,IDispatch *pDispatchObjectHit)
{
	return E_NOTIMPL;
}

STDMETHODIMP DuiLib::CWebBrowserUI::ShowMessage(HWND hwnd,LPOLESTR lpstrText,LPOLESTR lpstrCaption,DWORD dwType,LPOLESTR lpstrHelpFile,DWORD dwHelpContext,LRESULT *plResult)
{
	return E_NOTIMPL;
}

void DuiLib::CWebBrowserUI::SetWebBrowserEventHandler( CWebBrowserEventHandler* pEventHandler )
{
	if ( pEventHandler!=NULL && m_pWebBrowserEventHandler!=pEventHandler)
		m_pWebBrowserEventHandler=pEventHandler;
}

void DuiLib::CWebBrowserUI::OnErrPageReload()
{
	if(!m_szReloadUrl.IsEmpty())
		Navigate2(m_szReloadUrl.GetData(), m_hParentWnd);
}

bool DuiLib::CWebBrowserUI::OnJsMessage(DWORD dwMsg)
{
	PostMessage(m_hParentWnd,dwMsg,0,0);
	return true;
}

void DuiLib::CWebBrowserUI::Refresh2( int Level )
{
	CVariant vLevel;
	vLevel.vt=VT_I4;
	vLevel.intVal=Level;
	m_pWebBrowser2->Refresh2(&vLevel);
}

void DuiLib::CWebBrowserUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	if (_tcscmp(pstrName, _T("homepage")) == 0)
	{
		m_sHomePage = pstrValue;
	}
	else if (_tcscmp(pstrName, _T("autonavi"))==0)
	{
		m_bAutoNavi = (_tcscmp(pstrValue, _T("true")) == 0);
	}
	else
		CActiveXUI::SetAttribute(pstrName, pstrValue);
}

HWND DuiLib::CWebBrowserUI::GetInternetExplorerServerWnd()
{
	HWND hWnd = NULL;
	if(!m_pWebBrowser2) return hWnd;

	IServiceProvider* pServiceProvider = NULL;
	if (SUCCEEDED(m_pWebBrowser2->QueryInterface(
		IID_IServiceProvider, 
		(void**)&pServiceProvider)))
	{
		IOleWindow* pWindow = NULL;
		if (SUCCEEDED(pServiceProvider->QueryService(
			SID_SShellBrowser, 
			IID_IOleWindow,
			(void**)&pWindow)))
		{
			HWND hwndBrowser = NULL;
			if (SUCCEEDED(pWindow->GetWindow(&hwndBrowser)))
			{
				hWnd = hwndBrowser;
				// hwndBrowser is the handle of TabWindowClass
			}

			pWindow->Release();
		}

		pServiceProvider->Release();
	}
	return hWnd;
}

void DuiLib::CWebBrowserUI::SetHomePage(CDuiString szHomePage,bool bAutoNavi)
{
	m_sHomePage = szHomePage;
	m_bAutoNavi = bAutoNavi;
}

void DuiLib::CWebBrowserUI::NavigateHomePage()
{
	if (!m_sHomePage.IsEmpty())
		this->NavigateUrl(m_sHomePage);
}

void DuiLib::CWebBrowserUI::NavigateUrl( LPCTSTR lpszUrl )
{
	if (m_pWebBrowser2 && lpszUrl)
	{
		m_pWebBrowser2->Navigate((BSTR)SysAllocString(T2BSTR(lpszUrl)),NULL,NULL,NULL,NULL);
	}
}

LPCTSTR DuiLib::CWebBrowserUI::GetClass() const
{
	return _T("WebBrowserUI");
}

LPVOID DuiLib::CWebBrowserUI::GetInterface( LPCTSTR pstrName )
{
	if( _tcscmp(pstrName, DUI_CTR_WEBBROWSER) == 0 ) return static_cast<CWebBrowserUI*>(this);
	return CActiveXUI::GetInterface(pstrName);
}

void DuiLib::CWebBrowserUI::SetHomePage( LPCTSTR lpszUrl )
{
	m_sHomePage.Format(_T("%s"),lpszUrl);
}

LPCTSTR DuiLib::CWebBrowserUI::GetHomePage()
{
	return m_sHomePage;
}

void DuiLib::CWebBrowserUI::SetAutoNavigation( bool bAuto /*= TRUE*/ )
{
	if (m_bAutoNavi==bAuto)	return;

	m_bAutoNavi=bAuto;
}

bool DuiLib::CWebBrowserUI::IsAutoNavigation()
{
	return m_bAutoNavi;
}

STDMETHODIMP DuiLib::CWebBrowserUI::QueryService( REFGUID guidService, REFIID riid, void** ppvObject )
{
	HRESULT hr = E_NOINTERFACE;
	*ppvObject = NULL;

	if ( guidService == SID_SDownloadManager && riid == IID_IDownloadManager)
	{
		*ppvObject = this;
		return S_OK;
	}

	return hr;
}

HRESULT DuiLib::CWebBrowserUI::RegisterEventHandler( BOOL inAdvise )
{
	CComPtr<IWebBrowser2> pWebBrowser;
	CComPtr<IConnectionPointContainer>  pCPC;
	CComPtr<IConnectionPoint> pCP;
	HRESULT hr = GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
	if (FAILED(hr))
		return hr;
	hr=pWebBrowser->QueryInterface(IID_IConnectionPointContainer,(void **)&pCPC);
	if (FAILED(hr))
		return hr;
	hr=pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2,&pCP);
	if (FAILED(hr))
		return hr;

	if (inAdvise)
	{
		hr = pCP->Advise((IDispatch*)this, &m_dwCookie);
	}
	else
	{
		hr = pCP->Unadvise(m_dwCookie);
	}
	return hr; 
}

DISPID DuiLib::CWebBrowserUI::FindId( IDispatch *pObj, LPOLESTR pName )
{
	DISPID id = 0;
	if(FAILED(pObj->GetIDsOfNames(IID_NULL,&pName,1,LOCALE_SYSTEM_DEFAULT,&id))) id = -1;
	return id;
}

HRESULT DuiLib::CWebBrowserUI::InvokeMethod( IDispatch *pObj, LPOLESTR pMehtod, VARIANT *pVarResult, VARIANT *ps, int cArgs )
{
	DISPID dispid = FindId(pObj, pMehtod);
	if(dispid == -1) return E_FAIL;

	DISPPARAMS dispparams;
	dispparams.cArgs = cArgs;
	dispparams.rgvarg = ps;
	dispparams.cNamedArgs = 0;
	dispparams.rgdispidNamedArgs = NULL;

	return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispparams, pVarResult, NULL, NULL);
}

HRESULT DuiLib::CWebBrowserUI::GetProperty( IDispatch *pObj, LPOLESTR pName, VARIANT *pValue )
{
	DISPID dispid = FindId(pObj, pName);
	if(dispid == -1) return E_FAIL;

	DISPPARAMS ps;
	ps.cArgs = 0;
	ps.rgvarg = NULL;
	ps.cNamedArgs = 0;
	ps.rgdispidNamedArgs = NULL;

	return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &ps, pValue, NULL, NULL);
}

HRESULT DuiLib::CWebBrowserUI::SetProperty( IDispatch *pObj, LPOLESTR pName, VARIANT *pValue )
{
	DISPID dispid = FindId(pObj, pName);
	if(dispid == -1) return E_FAIL;

	DISPPARAMS ps;
	ps.cArgs = 1;
	ps.rgvarg = pValue;
	ps.cNamedArgs = 0;
	ps.rgdispidNamedArgs = NULL;

	return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT, &ps, NULL, NULL, NULL);
}

IDispatch* DuiLib::CWebBrowserUI::GetHtmlWindow()
{
	IDispatch* pDp =  NULL;
	HRESULT hr;
	if (m_pWebBrowser2)
		hr=m_pWebBrowser2->get_Document(&pDp);

	if (FAILED(hr))
		return NULL;

    CComQIPtr<IHTMLDocument2> pHtmlDoc2 = pDp;

	if (pHtmlDoc2 == NULL)
		return NULL;

	hr=pHtmlDoc2->get_parentWindow(&m_pHtmlWnd2);

	if (FAILED(hr))
		return NULL;

	IDispatch *pHtmlWindown = NULL;
	hr=m_pHtmlWnd2->QueryInterface(IID_IDispatch, (void**)&pHtmlWindown);
	if (FAILED(hr))
		return NULL;

	return pHtmlWindown;
}

IWebBrowser2* DuiLib::CWebBrowserUI::GetWebBrowser2( void )
{
	return m_pWebBrowser2;
}

HRESULT STDMETHODCALLTYPE DuiLib::CWebBrowserUI::QueryStatus( __RPC__in_opt const GUID *pguidCmdGroup, ULONG cCmds, __RPC__inout_ecount_full(cCmds ) OLECMD prgCmds[ ], __RPC__inout_opt OLECMDTEXT *pCmdText )
{
	HRESULT hr = OLECMDERR_E_NOTSUPPORTED;
	return hr;
}

HRESULT STDMETHODCALLTYPE DuiLib::CWebBrowserUI::Exec( __RPC__in_opt const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, __RPC__in_opt VARIANT *pvaIn, __RPC__inout_opt VARIANT *pvaOut )
{
	HRESULT hr = S_OK;

	if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
	{

		switch (nCmdID) 
		{

		case OLECMDID_SHOWSCRIPTERROR:
			{
				IHTMLDocument2*             pDoc = NULL;
				IHTMLWindow2*               pWindow = NULL;
				IHTMLEventObj*              pEventObj = NULL;
				BSTR                        rgwszNames[5] = 
				{ 
					SysAllocString(L"errorLine"),
					SysAllocString(L"errorCharacter"),
					SysAllocString(L"errorCode"),
					SysAllocString(L"errorMessage"),
					SysAllocString(L"errorUrl")
				};
				DISPID                      rgDispIDs[5];
				VARIANT                     rgvaEventInfo[5];
				DISPPARAMS                  params;
				BOOL                        fContinueRunningScripts = true;
				int                         i;

				params.cArgs = 0;
				params.cNamedArgs = 0;

				// Get the document that is currently being viewed.
				hr = pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, (void **) &pDoc);    
				// Get document.parentWindow.
				hr = pDoc->get_parentWindow(&pWindow);
				pDoc->Release();
				// Get the window.event object.
				hr = pWindow->get_event(&pEventObj);
				// Get the error info from the window.event object.
				for (i = 0; i < 5; i++) 
				{  
					// Get the property's dispID.
					hr = pEventObj->GetIDsOfNames(IID_NULL, &rgwszNames[i], 1, 
						LOCALE_SYSTEM_DEFAULT, &rgDispIDs[i]);
					// Get the value of the property.
					hr = pEventObj->Invoke(rgDispIDs[i], IID_NULL,
						LOCALE_SYSTEM_DEFAULT,
						DISPATCH_PROPERTYGET, &params, &rgvaEventInfo[i],
						NULL, NULL);
					SysFreeString(rgwszNames[i]);
				}

				// At this point, you would normally alert the user with 
				// the information about the error, which is now contained
				// in rgvaEventInfo[]. Or, you could just exit silently.

				(*pvaOut).vt = VT_BOOL;
				if (fContinueRunningScripts)
				{
					// Continue running scripts on the page.
					(*pvaOut).boolVal = VARIANT_TRUE;
				}
				else
				{
					// Stop running scripts on the page.
					(*pvaOut).boolVal = VARIANT_FALSE;   
				} 
				break;
			}
		default:
			hr = OLECMDERR_E_NOTSUPPORTED;
			break;
		}
	}
	else
	{
		hr = OLECMDERR_E_UNKNOWNGROUP;
	}
	return (hr);
}

bool DuiLib::CWebBrowserUI::ExecJsFun( const wstring& lpJsFun, const vector<wstring>& params )
{
	if ( NULL == m_pWebBrowser2 )
		return false;
	CComPtr<IDispatch> pDoc;
	HRESULT hr = m_pWebBrowser2->get_Document(&pDoc);
	if ( FAILED(hr) )
		return false;
	CComQIPtr<IHTMLDocument2> pDoc2=pDoc;
	if ( NULL == pDoc2 )
		return false;
	CComQIPtr<IDispatch> pScript;
	hr = pDoc2->get_Script(&pScript);
	if ( FAILED(hr) )
		return false;
	DISPID id = NULL;
	CComBSTR bstrFun(lpJsFun.c_str());
	hr = pScript->GetIDsOfNames(IID_NULL, &bstrFun, 1, LOCALE_SYSTEM_DEFAULT, &id);
	if ( FAILED(hr) )
		return false;
	DISPPARAMS dispParams;
	memset(&dispParams, 0, sizeof(DISPPARAMS));
	int nParamCount	= params.size();
	if ( nParamCount > 0 )
	{
		dispParams.cArgs	=nParamCount;
		dispParams.rgvarg	=new VARIANT[nParamCount];
		for (int i=0; i<nParamCount; ++i )
		{
			const wstring& str = params[nParamCount-1-i];
			CComBSTR bstr(str.c_str());
			bstr.CopyTo(&dispParams.rgvarg[i].bstrVal);
			dispParams.rgvarg[i].vt	= VT_BSTR;
		}
	}
	EXCEPINFO execInfo;
	memset(&execInfo, 0, sizeof(EXCEPINFO));
	VARIANT vResult;
	UINT uArgError = (UINT)-1;
	hr = pScript->Invoke(id, IID_NULL, 0, DISPATCH_METHOD, &dispParams, &vResult, &execInfo, &uArgError);
	delete[] dispParams.rgvarg;
	if ( FAILED(hr) )
		return false;
	return true;
}
