#pragma once
#include <vector>
#include "Player\PlayerUI.h"
#include "WndPlayBar.h"

class CWndMain :
	public WindowImplBase,
	public IPlayerCallback
{
	enum
	{
		TIMER_ID_BEGIN,
		TIMER_ID_POS = 100,
		TIMER_ID_MOUSE,
	};
public:
	CWndMain(const wstring& strUrl, const wstring& strTitle);
	~CWndMain();
	virtual void ShowWindow(bool bShow = true, bool bTakeFocus = true);

protected:
	virtual LPCTSTR		GetWindowClassName()const	{ return L"XbyMainWndClass"; }
	virtual CDuiString	GetSkinFolder()				{ return L"PlayerSkin"; }
	virtual CDuiString	GetSkinFile()				{ return L"WndMain.xml"; }
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual void OnClick(TNotifyUI& msg);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnMsgSetPlayProgress(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgSetVolumeProgress(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgPlay(WPARAM wParam, LPARAM lParam);

	void OnMouseOverPlayer(HWND hWnd);
	void OnMouseLeavePlayer(HWND hWnd);
	void OnMouseMovePlayer(HWND hWnd);
	void OnMouseLButtonUp(HWND hWnd);

	BOOL OnOpenVideo();
	void UpdatePlayBarPos();

	virtual void OnOpenComplete(void* lpUserParam, long lDuiration);
	virtual void OnStateChange(void* lpUserParam, PlayerState state);
	virtual void OnDownloadCodec(void* lpUserParam, char* pCodecPath);
	virtual void OnVolumeChange(void* lpUserParam, long nVolume);
	virtual void OnSeekCompleted(void* lpUserParam, long nPosition);
	virtual void OnBuffer(void* lpUserParam, long nPercent);
	virtual LRESULT OnMsg(void* lpUserParam, HWND hWnd, LONG nMessage, LONG wParam, LONG lParam);
	virtual LRESULT OnMenuClick(WORD wId, BOOL& bHandled);

	static void DurationToString(long nDuration, OUT wstring& strTime);
	static LRESULT WINAPI CallWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);


private:
	bool m_bCatch;
	bool m_bMouse;
	CPlayerUI*	m_pPlayer;
	HBITMAP m_hBmpLogo;
	HWND	m_hPlayBarWnd;
	CWndPlayBar* m_pPlayBar;
	CControlUI* m_pLblTitle;
	wstring m_strUrl;
	wstring m_strTitle;
	__int64 m_nLastMoveTime;
	static WNDPROC m_OldWndProc;
};

