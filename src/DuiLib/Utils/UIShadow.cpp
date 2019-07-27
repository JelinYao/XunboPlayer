#ifndef __UISHADOW_H__
#define __UISHADOW_H__

#include "StdAfx.h"
#include "math.h"

namespace DuiLib
{
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000
#endif

#ifndef AC_SRC_ALPHA
#define AC_SRC_ALPHA 0x01
#endif

#ifndef ULW_ALPHA
#define ULW_ALPHA 0x00000002
#endif


#ifndef WM_DWMCOMPOSITIONCHANGED
#define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif

	CUIShadow::pfnUpdateLayeredWindow CUIShadow::s_UpdateLayeredWindow = NULL;
	CUIShadow::pfnDwmIsCompositionEnabled CUIShadow::s_DwmIsCompositionEnabled = NULL;
	const TCHAR *strWndClassName = _T("PerryShadowWnd");
	HINSTANCE CUIShadow::s_hInstance = (HINSTANCE)INVALID_HANDLE_VALUE;
	std::map<HWND, CUIShadow *> CUIShadow::s_Shadowmap;

	CUIShadow::CUIShadow(void)
	{
		m_szImage = _T("");
		m_pMemImage = NULL;
		m_bVisible = false;
		m_bPrepared = false;
		m_bUpdate = false;
		m_Status |= SS_VISABLE;
		m_Status |= SS_PARENTVISIBLE;
		m_bTrans = false;
	}

	CUIShadow::~CUIShadow(void)
	{
	}

	bool CUIShadow::Initialize(HINSTANCE hInstance)
	{
		if (NULL != s_UpdateLayeredWindow)
			return false;

		HMODULE hSysDll = LoadLibrary(_T("USER32.DLL"));
		s_UpdateLayeredWindow = 
			(pfnUpdateLayeredWindow)GetProcAddress(hSysDll, 
			"UpdateLayeredWindow");

		if (NULL == s_UpdateLayeredWindow)
			return false;

		hSysDll = LoadLibrary(_T("dwmapi.dll"));
		if(hSysDll)
		{
			s_DwmIsCompositionEnabled = 
				(pfnDwmIsCompositionEnabled)GetProcAddress(hSysDll, 
				"DwmIsCompositionEnabled");
		}

		s_hInstance = hInstance;

		WNDCLASSEX wcex;

		memset(&wcex, 0, sizeof(wcex));

		wcex.cbSize = sizeof(WNDCLASSEX); 
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= DefWindowProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= NULL;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= strWndClassName;
		wcex.hIconSm		= NULL;

		RegisterClassEx(&wcex);

		return true;
	}

	bool CUIShadow::Create(HWND hParentWnd,CPaintManagerUI* pManager)
	{
		m_pManager = pManager;
		m_hParentWnd = hParentWnd;
		if(NULL == s_UpdateLayeredWindow)
			return false;

		_ASSERT(s_hInstance != INVALID_HANDLE_VALUE);

		_ASSERT(s_Shadowmap.find(hParentWnd) == s_Shadowmap.end());
		s_Shadowmap[hParentWnd] = this;

		m_hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, strWndClassName, NULL,
			/*WS_VISIBLE | WS_CAPTION | */WS_POPUPWINDOW | WS_VISIBLE,
			CW_USEDEFAULT, 0, 0, 0, hParentWnd, NULL, s_hInstance, NULL);

		ShowWindow(m_hWnd, SW_HIDE);
		m_Status = SS_ENABLED;
		BOOL bAero = FALSE;
		if(s_DwmIsCompositionEnabled)
			s_DwmIsCompositionEnabled(&bAero);
		if (bAero)
			m_Status |= SS_DISABLEDBYAERO;

		Show(hParentWnd);
		RECT WndRect;
		GetWindowRect(hParentWnd, &WndRect);
		int nShadWndWid = WndRect.right - WndRect.left + m_rcBorder.left + m_rcBorder.right;
		int nShadWndHei = WndRect.bottom - WndRect.top + m_rcBorder.top + m_rcBorder.bottom;

		POINT ptDst = {WndRect.left - m_rcBorder.left, WndRect.top - m_rcBorder.top};

		MoveWindow(m_hWnd, ptDst.x, ptDst.y, nShadWndWid, nShadWndHei, FALSE);

		m_OriParentProc = GetWindowLong(hParentWnd, GWL_WNDPROC);
		LONG ss = SetWindowLong(hParentWnd, GWL_WNDPROC, (LONG)ParentProc);
		return true;
	}

	LRESULT CALLBACK CUIShadow::ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		_ASSERT(s_Shadowmap.find(hwnd) != s_Shadowmap.end());
		CUIShadow *pThis = s_Shadowmap[hwnd];
		WNDPROC pDefProc = (WNDPROC)pThis->m_OriParentProc;

		switch(uMsg)
		{
		case WM_MOVE:
			if(pThis->m_bVisible)
			{
				RECT WndRect;
				GetWindowRect(hwnd, &WndRect);
				int nx = WndRect.left - pThis->m_rcBorder.left;
				int ny = WndRect.top - pThis->m_rcBorder.top;
				int nwidth = WndRect.right + pThis->m_rcBorder.right;
				int nheight = WndRect.bottom + pThis->m_rcBorder.bottom;
				nwidth = nwidth - nx;
				nheight = nheight - ny;
				
				if(pThis->m_bVisible){
					::MoveWindow(pThis->m_hWnd,nx,ny,nwidth,nheight,TRUE);
				}else{
					::MoveWindow(pThis->m_hWnd,nx,ny,nwidth,nheight,FALSE);
				}
//  				SetWindowPos(pThis->m_hWnd, 0,
//  					WndRect.left - pThis->m_rcBorder.left, WndRect.top - pThis->m_rcBorder.top,
//  					WndRect.right + pThis->m_rcBorder.right, WndRect.bottom + pThis->m_rcBorder.bottom, style);
			}
			break;
		case WM_SIZE:
// 			if(pThis->m_Status & SS_ENABLED && !(pThis->m_Status & SS_DISABLEDBYAERO))
// 			{
			{
				if(SIZE_MAXIMIZED == wParam || SIZE_MINIMIZED == wParam)
				{
					ShowWindow(pThis->m_hWnd, SW_HIDE);
					pThis->m_Status &= ~SS_VISABLE;
					pThis->m_bVisible = false;
				}
				else
				{
					LONG lParentStyle = GetWindowLong(hwnd, GWL_STYLE);
					if(WS_VISIBLE & lParentStyle)
					{
						pThis->m_Status |= SS_PARENTVISIBLE;
						if(!(pThis->m_Status & SS_VISABLE))
						{
							if(pThis->m_bPrepared){
								pThis->m_Status |= SS_VISABLE;
								pThis->m_bVisible = true;
								pThis->Update(pThis->m_hParentWnd);
								ShowWindow(pThis->m_hWnd, SW_SHOWNA);
								pThis->m_bUpdate = true;
							}
						}
						else if(LOWORD(lParam) > LOWORD(pThis->m_WndSize) || HIWORD(lParam) > HIWORD(pThis->m_WndSize))
							pThis->m_bUpdate = true;
						else
							pThis->Update(pThis->m_hParentWnd);
					}

				}
				pThis->m_WndSize = lParam;
			}
			break;

		case WM_PAINT:
			{
				if(pThis->m_bUpdate)
				{
					pThis->Update(pThis->m_hParentWnd);
// 					if(pThis->m_bVisible){
// 						pThis->m_bUpdate = false;
// 					}
					pThis->m_bUpdate = false;
				}
				break;
			}
		case WM_EXITSIZEMOVE:
			if(pThis->m_bVisible)
			{
				pThis->Update(pThis->m_hParentWnd);
			}
			break;

		case WM_SHOWWINDOW:
			//if(pThis->m_Status & SS_ENABLED && !(pThis->m_Status & SS_DISABLEDBYAERO))
			//{
			{
				LRESULT lResult =  pDefProc(hwnd, uMsg, wParam, lParam);
				if(!wParam)
				{
					ShowWindow(pThis->m_hWnd, SW_HIDE);
					pThis->m_Status &= ~(SS_VISABLE | SS_PARENTVISIBLE);
					pThis->m_bVisible = false;
				}
				else
				{
					if(pThis->m_bPrepared){
						pThis->m_bUpdate = true;
						pThis->m_bVisible = true;
						//pThis->m_bVisible = true;
						pThis->m_Status |= SS_VISABLE;
						pThis->Update(pThis->m_hParentWnd);
						ShowWindow(pThis->m_hWnd, SW_SHOWNA);
					}
				}
				return lResult;
			}
			break;

		case WM_DESTROY:
			DestroyWindow(pThis->m_hWnd);
			break;

		case WM_NCDESTROY:
			s_Shadowmap.erase(hwnd);
			break;

		case WM_DWMCOMPOSITIONCHANGED:
			{
				BOOL bAero = FALSE;
				if(s_DwmIsCompositionEnabled)	
					s_DwmIsCompositionEnabled(&bAero);
				if (bAero)
					pThis->m_Status |= SS_DISABLEDBYAERO;
				else
					pThis->m_Status &= ~SS_DISABLEDBYAERO;

				pThis->Show(hwnd);
			}
			break;

		}
		return pDefProc(hwnd, uMsg, wParam, lParam);

	}

	void CUIShadow::Update(HWND hParent)
	{
		RECT WndRect;
		GetWindowRect(hParent, &WndRect);
		int nShadWndWid = WndRect.right - WndRect.left + m_rcBorder.left + m_rcBorder.right;
		int nShadWndHei = WndRect.bottom - WndRect.top + m_rcBorder.top + m_rcBorder.bottom;
		

		POINT ptDst = {WndRect.left - m_rcBorder.left, WndRect.top - m_rcBorder.top};
		POINT ptSrc = {0, 0};
		SIZE WndSize = {nShadWndWid, nShadWndHei};
		BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

		if(m_pMemImage){
			if(!m_pMemImage->IsNull()) m_pMemImage->Destroy();
			m_pMemImage = NULL;
		}
		m_pMemImage = new CDuiImage();
		m_pMemImage->CreateEmptyImage(nShadWndWid,nShadWndHei,32);
		
		RECT rcPaint;
		rcPaint.left = 0;
		rcPaint.right = nShadWndWid;
		rcPaint.top = 0;
		rcPaint.bottom = nShadWndHei;


		CRenderEngine::DrawImageString(m_pMemImage->GetDC(), m_pManager, rcPaint, rcPaint, m_szImage, NULL);
		m_pMemImage->ReleaseDC();
		
		MoveWindow(m_hWnd, ptDst.x, ptDst.y, nShadWndWid, nShadWndHei, FALSE);
	
		BOOL	bRet = s_UpdateLayeredWindow(m_hWnd, NULL, &ptDst, &WndSize, m_pMemImage->GetDC(),
				&ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
	

		m_pMemImage->ReleaseDC();
		if(m_pMemImage){
			if(!m_pMemImage->IsNull()) m_pMemImage->Destroy();
			m_pMemImage = NULL;
		}
		_ASSERT(bRet);
	}

	void CUIShadow::Show(HWND hParentWnd)
	{
		m_Status &= SS_ENABLED | SS_DISABLEDBYAERO;
		if((m_Status & SS_ENABLED) && !(m_Status & SS_DISABLEDBYAERO))
		{
			LONG lParentStyle = GetWindowLong(hParentWnd, GWL_STYLE);
			if(WS_VISIBLE & lParentStyle){
				m_Status |= SS_PARENTVISIBLE;
				if(!((WS_MAXIMIZE | WS_MINIMIZE) & lParentStyle)){
					m_bVisible = true;
					m_Status |= SS_VISABLE;
				}

			}
		}

		if(m_Status & SS_VISABLE){
			if(m_bVisible)
				ShowWindow(m_hWnd, SW_SHOWNA);
			else
				ShowWindow(m_hWnd, SW_HIDE);
			Update(hParentWnd);
		}
		else
			ShowWindow(m_hWnd, SW_HIDE);
	}

	void CUIShadow::SetImage(CDuiString pImage)
	{
		m_szImage = pImage;
	}

	void CUIShadow::SetBorder(RECT rcBorder)
	{
		m_rcBorder.left = rcBorder.left;
		m_rcBorder.right = rcBorder.right;
		m_rcBorder.top = rcBorder.top;
		m_rcBorder.bottom = rcBorder.bottom;
	}

	void CUIShadow::SetPrepared(bool bPrepared)
	{
		m_bPrepared = bPrepared;
	}

	void CUIShadow::SetVisible(bool bVisible)
	{
		m_bVisible = bVisible;
		if(m_bVisible){
			m_bUpdate = true;
			m_bPrepared = true;
			m_Status |= SS_VISABLE;
			m_Status |= SS_PARENTVISIBLE;
			//Show(m_hWnd);
			ShowWindow(m_hWnd, SW_SHOWNA);
			Update(m_hParentWnd);
			//::RedrawWindow(m_hWnd,NULL, NULL,   RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
		}
	}
}
