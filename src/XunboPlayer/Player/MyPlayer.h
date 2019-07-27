#pragma once
#include <atlbase.h>
#include <oaidl.h>
#include "IPlayerCallback.h"
#import "../../lib/APlayer.dll"

enum PLAY_STATE
{
	PS_READY = 0,
	PS_OPENING = 1,
	PS_PAUSING = 2,
	PS_PAUSED = 3,
	PS_PLAYING = 4,
	PS_PLAY = 5,
	PS_CLOSING = 6,
};


class CMyPlayer 
	: public IDispatch
{
public:
	CMyPlayer();
	~CMyPlayer();
	void SetPlayerCallback(IPlayerCallback* pCallback, void* lpParam);
	void DetachCallback();

protected:
	// IUnknown
	ULONG __stdcall AddRef(void);
	ULONG __stdcall Release(void);
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject);

	// IDispatch
	STDMETHOD(GetTypeInfoCount)(unsigned int FAR* pctinfo);
	STDMETHOD(GetTypeInfo)(unsigned int iTInfo, LCID lcid, ITypeInfo FAR* FAR* ppTInfo);
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR FAR* FAR* rgszNames, unsigned int cNames, LCID lcid, DISPID FAR* rgDispId);
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR* pDispParams, VARIANT FAR* pVarResult, EXCEPINFO FAR* pExcepInfo, unsigned int FAR* puArgErr);

	// aplayer event handler
	virtual	HRESULT	OnMessage(LONG nMessage, LONG wParam, LONG lParam);
	virtual	HRESULT	OnStateChanged(LONG nOldState, LONG nNewState);
	virtual	HRESULT	OnSeekCompleted(LONG nPosition);
	virtual	HRESULT	OnBuffer(LONG nPercent);
	virtual	HRESULT	OnVideoSizeChanged(void);

public:
	int						m_nPosition;
	long					m_nDuration;
	APlayer3Lib::IPlayer *	m_pAPlayer;
	IConnectionPoint *		m_pConnectionPoint;
	DWORD					m_dwCookie;
	HWND					m_hAPlayerWnd;
	HWND					m_hParentWnd;
	HMODULE					m_hModule;

public:
	void SetPos(const RECT& rc);
	int	GetConfigInt(int nConfigId);
	void Initialize(HWND hWndParent);
	BOOL GetPathFromFullName(const TCHAR * pcszFullName, TCHAR * pszPath);
	BOOL CreateAPlayerWindow();

protected:
	void Close();
	HRESULT	CreateInstanceFromFile(const TCHAR * pcszPath, REFCLSID rclsid, REFIID riid, IUnknown * pUnkOuter, LPVOID * ppv);
	//È«ÆÁ¹¦ÄÜ
	void EnterFullScreen(void);
	void ExitFullScreen(void);
	static LONG WINAPI FullScreenWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	RECT	m_rcPos;
	HWND	m_hFullScreenWnd;
	void	*m_lpUserParam;
	IPlayerCallback	*m_pCallback;
};

