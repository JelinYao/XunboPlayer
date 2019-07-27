#ifndef WIN_IRREGULAR_WND_HPP
#define WIN_IRREGULAR_WND_HPP

#include "StdAfx.h"

namespace DuiLib
{

	const TCHAR* __className  =  _T("__IrregularWindowClass");
	const TCHAR* __windowName =  _T("__IrregularWindow");

	ULONG_PTR CIrregularWindow::gdiplusToken = 0;

	HHOOK CIrregularWindow::hhk = NULL;

	CIrregularWindow::CIrregularWindow(const HBITMAP hBitmap,DWORD dwAttachWndTransColor)
		:m_hWnd(NULL)
		,m_hAttachWnd(NULL)
		,m_dwAttachWndTransColor(dwAttachWndTransColor)
	{
		if(hBitmap != NULL)
		{
			m_hBitmap = hBitmap;

			if(!RegisterWindowClass() || !Create())
			{
				assert(false && _T("背景窗体创建失败!"));
			}
		}

		setlocale(LC_ALL,"");
	}


	bool CIrregularWindow::RegisterWindowClass()
	{
		WNDCLASS wc = { 0 };
		wc.style = 0;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = NULL;
		wc.lpfnWndProc = CIrregularWindow::__WndProc;
		wc.hInstance = (HINSTANCE)::GetModuleHandle(NULL);
		wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName  = NULL;
		wc.lpszClassName =__className;
		ATOM ret = ::RegisterClass(&wc);

		assert(ret != NULL);

		return ret != NULL;
	}

	bool CIrregularWindow::Create()
	{
		m_hWnd = ::CreateWindowEx(WS_EX_LAYERED, __className, _T(""),WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 
			CW_USEDEFAULT, CW_USEDEFAULT, 
			NULL, NULL, (HINSTANCE)::GetModuleHandle(NULL), 0);

		if(m_hWnd == NULL || !::IsWindow(m_hWnd))
			return false;

		ShowWindow(m_hWnd,SW_SHOWNORMAL);

		return true;
	}

	bool CIrregularWindow::AttachWindow(HWND hWnd)
	{
		if(m_hAttachWnd != NULL)
		{
			assert(false && _T("不能重复AttachWindow!"));
			return false;
		}

		if(!IsWindow(hWnd))
		{
			assert(false && _T("无效的窗口句柄!"));
			return false;
		}

		m_hAttachWnd = hWnd;

		SetWindowLong(m_hWnd,GWL_USERDATA,(LONG)m_hAttachWnd);

		//将附加窗体样式 加上WS_EX_LAYERED
		DWORD dwAttachWndStyleEx = GetWindowLong(m_hAttachWnd,GWL_EXSTYLE);

		::SetWindowLong(m_hAttachWnd,GWL_EXSTYLE,dwAttachWndStyleEx | WS_EX_LAYERED);

		SetBackground(m_hBitmap);

		SetAttachWndTransColor(m_dwAttachWndTransColor);

		return true;
	}

	void* CIrregularWindow::GetImage(HBITMAP hBitmap,UINT bits)
	{
		HANDLE hdib;
		HDC hdc;
		BITMAP bitmap;
		UINT wLineLen;
		DWORD dwSize;
		DWORD wColSize;
		LPBITMAPINFOHEADER lpbi;
		LPBYTE lpBits;

		GetObject(hBitmap, sizeof(BITMAP), &bitmap);

		wLineLen = (bitmap.bmWidth*bits+31)/32*4;
		wColSize = sizeof(RGBQUAD)*((bits <= 8) ? 1<<bits : 0);
		dwSize = sizeof(BITMAPINFOHEADER) + wColSize + (DWORD)(UINT)wLineLen*(DWORD)(UINT)bitmap.bmHeight;

		hdib = GlobalAlloc(GHND,dwSize);
		if (!hdib)
			return hdib;

		lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);   

		lpbi->biSize = sizeof(BITMAPINFOHEADER);
		lpbi->biWidth = bitmap.bmWidth;
		lpbi->biHeight = bitmap.bmHeight;
		lpbi->biPlanes = 1;
		lpbi->biBitCount = (WORD)bits;
		lpbi->biCompression = BI_RGB;
		lpbi->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize;
		lpbi->biXPelsPerMeter = 0;
		lpbi->biYPelsPerMeter = 0;
		lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;
		lpbi->biClrImportant = 0;

		lpBits = (LPBYTE)(lpbi+1)+wColSize;

		hdc = CreateCompatibleDC(NULL);

		GetDIBits(hdc, hBitmap, 0, bitmap.bmHeight, lpBits, (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

		lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;

		DeleteDC(hdc);
		GlobalUnlock(hdib);
		
		IStream* pStmBmp = NULL;
		CreateStreamOnHGlobal(hdib, FALSE, &pStmBmp);
		
		if (pStmBmp == NULL) 
		{
		//	GlobalFree(hdib);
			return NULL;
		}
		Gdiplus::Image* imImage = NULL;
		imImage = Gdiplus::Image::FromStream(pStmBmp, FALSE);
		if (imImage == NULL) 
		{
		//	GlobalFree(hdib);
			return NULL;
		}
		return imImage;
	}

	void CIrregularWindow::SetBackground(const HBITMAP hBitmap)
	{
		if(hBitmap != NULL)
		{
			m_hBitmap = hBitmap;
		}

		if(m_hBitmap)
		{
//#ifdef UNICODE
			Gdiplus::Image* pImage = (Gdiplus::Image*)GetImage(hBitmap);
			//pImage->FromStream()
// #else
// 			size_t szHasConv = 0;
// 			DWORD dwSize = m_strBackImg.GetLength() + 1;
// 			wchar_t* pImgPath = new wchar_t[dwSize];
// 			memset((void*)pImgPath,0,sizeof(wchar_t) * dwSize);
// 			::mbstowcs_s(&szHasConv,pImgPath,dwSize,m_strBackImg.GetData(),m_strBackImg.GetLength());
// 			Gdiplus::Image* pImage = Gdiplus::Image::FromFile(pImgPath);
// 			delete [] pImgPath;	
// #endif

			if(pImage)
			{
				BLENDFUNCTION blendFunc;
				blendFunc.BlendOp = 0;
				blendFunc.BlendFlags = 0;
				blendFunc.AlphaFormat = 1;
				blendFunc.SourceConstantAlpha = 255;//AC_SRC_ALPHA

				SIZE sizeWindow = { pImage->GetWidth(), pImage->GetHeight()};

				HDC hDC = ::GetDC(m_hWnd);

				HDC hdcMemory = CreateCompatibleDC(hDC);

				HBITMAP hBitMap = CreateCompatibleBitmap(hDC, sizeWindow.cx, sizeWindow.cy);

				::SelectObject(hdcMemory, hBitMap);

				RECT rcWindow;
				GetWindowRect(m_hWnd,&rcWindow);

				BITMAPINFOHEADER stBmpInfoHeader = { 0 };   
				int nBytesPerLine = ((sizeWindow.cx * 32 + 31) & (~31)) >> 3;
				stBmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);   
				stBmpInfoHeader.biWidth = sizeWindow.cx;   
				stBmpInfoHeader.biHeight = sizeWindow.cy;   
				stBmpInfoHeader.biPlanes = 1;   
				stBmpInfoHeader.biBitCount = 32;   
				stBmpInfoHeader.biCompression = BI_RGB;   
				stBmpInfoHeader.biClrUsed = 0;   
				stBmpInfoHeader.biSizeImage = nBytesPerLine * sizeWindow.cy;   

				PVOID pvBits = NULL;   

				HBITMAP hbmpMem = ::CreateDIBSection(NULL, (PBITMAPINFO)&stBmpInfoHeader, DIB_RGB_COLORS, &pvBits, NULL, 0);

				assert(hbmpMem != NULL);

				memset( pvBits, 0, sizeWindow.cx * 4 * sizeWindow.cy);

				if(hbmpMem)   
				{   
					HGDIOBJ hbmpOld = ::SelectObject( hdcMemory, hbmpMem);

					POINT ptWinPos = { rcWindow.left, rcWindow.top };

					Gdiplus::Graphics graph(hdcMemory);

					graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);

					graph.DrawImage(pImage, 0, 0, sizeWindow.cx, sizeWindow.cy);

					HMODULE hFuncInst = LoadLibrary(_T("User32.DLL"));

					typedef BOOL (WINAPI *MYFUNC)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);          

					MYFUNC UpdateLayeredWindow;

					UpdateLayeredWindow = (MYFUNC)::GetProcAddress(hFuncInst, "UpdateLayeredWindow");

					POINT ptSrc = { 0, 0};

					//不会发送 WM_SIZE和WM_MOVE消息
					if(!UpdateLayeredWindow(m_hWnd, hDC, &ptWinPos, &sizeWindow, hdcMemory, &ptSrc, 0, &blendFunc, ULW_ALPHA))
					{
						TCHAR tmp[255] = {_T('\0')};
#ifdef UNICODE
#define	MySprintf swprintf_s
#else
#define MySprintf sprintf_s
#endif
						MySprintf(tmp,255,__T("UpdateLayeredWindow 调用失败,错误代码:%u"),GetLastError());

						MessageBox(m_hWnd,tmp,_T("提示"),MB_OK);
					}

					//::SendMessage(m_hWnd,WM_SIZE,0,MAKELONG(sizeWindow.cy,sizeWindow.cx));

					//::SendMessage(m_hWnd,WM_MOVE,0,MAKELONG(ptSrc.y,ptSrc.x));

					graph.ReleaseHDC(hdcMemory);

					::SelectObject( hdcMemory, hbmpOld);   

					::DeleteObject(hbmpMem); 
				}

				::DeleteDC(hdcMemory);

				::DeleteDC(hDC);
			}
			else
			{
				assert(false && _T("背景图片打开失败!"));
			}
		}
	}

	void CIrregularWindow::SetAttachWndTransColor(DWORD dwColor)
	{
		m_dwAttachWndTransColor = dwColor;

		HMODULE hInst = LoadLibrary(_T("User32.DLL")); 

		typedef BOOL (WINAPI *MYFUNC)(HWND, COLORREF, BYTE, DWORD); 

		MYFUNC SetLayeredWindowAttributes = NULL; 

		SetLayeredWindowAttributes = (MYFUNC)::GetProcAddress(hInst, "SetLayeredWindowAttributes"); 

		if(SetLayeredWindowAttributes)
		{
			if(!SetLayeredWindowAttributes(m_hAttachWnd, m_dwAttachWndTransColor, 0, LWA_COLORKEY))
			{
				assert(false && _T("SetLayeredWindowAttributes 失败了!"));
			}
		}

		::FreeLibrary(hInst);
	}

	void CIrregularWindow::InitGDIplus()
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;

		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	}

	void CIrregularWindow::UnInitGDIplus()
	{
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}

	void CIrregularWindow::CenterWindow()
	{
		RECT rcDlg = {0};
		::GetWindowRect(m_hWnd, &rcDlg);
		RECT rcArea = { 0 };
		RECT rcCenter = { 0 };
		HWND hWndParent = ::GetParent(m_hWnd);
		HWND hWndCenter = ::GetWindowOwner(m_hWnd);
		::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
		if( hWndCenter == NULL ) rcCenter = rcArea; else ::GetWindowRect(hWndCenter, &rcCenter);

		int DlgWidth = rcDlg.right - rcDlg.left;
		int DlgHeight = rcDlg.bottom - rcDlg.top;

		// Find dialog's upper left based on rcCenter
		int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
		int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

		// The dialog is outside the screen, move it inside
		if( xLeft < rcArea.left ) xLeft = rcArea.left;
		else if( xLeft + DlgWidth > rcArea.right ) xLeft = rcArea.right - DlgWidth;
		if( yTop < rcArea.top ) yTop = rcArea.top;
		else if( yTop + DlgHeight > rcArea.bottom ) yTop = rcArea.bottom - DlgHeight;
		::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	LRESULT CALLBACK  CIrregularWindow::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		HWND hAttackWnd = (HWND)GetWindowLong(hWnd,GWL_USERDATA);
		switch(uMsg)
		{
		case WM_CREATE:
			{
				LONG styleValue = ::GetWindowLong(hWnd, GWL_STYLE);
				styleValue &= ~WS_CAPTION;
				styleValue &= ~WS_MAXIMIZEBOX; 
				styleValue &= ~WS_MINIMIZEBOX; 
				styleValue &= ~WS_THICKFRAME; 
				styleValue &= ~WS_BORDER; 
				styleValue &= ~WS_CAPTION;
				::SetWindowLong(hWnd, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			}
			break;
		case WM_SIZE:
			{
				short cx = LOWORD(lParam); 
				short cy = HIWORD(lParam);

				::SetWindowPos(hAttackWnd, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);
			}
			break;
		case WM_MOVE:
			{
				RECT rect;
				::GetWindowRect(hWnd,&rect);
				::SetWindowPos(hAttackWnd, NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			break;
		case WM_LBUTTONDOWN:
			{
				::SendMessage( hWnd, WM_SYSCOMMAND, 0xF012, 0);
				::SendMessage( hAttackWnd, WM_SYSCOMMAND, 0xF012, 0);
			}
			break;
		}

		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	}

}
