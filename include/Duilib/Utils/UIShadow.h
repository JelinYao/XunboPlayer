#ifndef __UISHADOW_H__
#define __UISHADOW_H__

#include <map>
using namespace std;

namespace DuiLib
{
	class CPaintManagerUI;
	class UILIB_API CUIShadow
	{
	public:
		CUIShadow(void);
		virtual ~CUIShadow(void);
	public:
		typedef BOOL (WINAPI *pfnUpdateLayeredWindow)(HWND hWnd, HDC hdcDst, POINT *pptDst,
			SIZE *psize, HDC hdcSrc, POINT *pptSrc, COLORREF crKey,
			BLENDFUNCTION *pblend, DWORD dwFlags);
		typedef HRESULT (WINAPI *pfnDwmIsCompositionEnabled)(BOOL *pfEnabled);
		static pfnUpdateLayeredWindow s_UpdateLayeredWindow;
		static pfnDwmIsCompositionEnabled s_DwmIsCompositionEnabled;
		
		static bool Initialize(HINSTANCE hInstance);
		
		bool Create(HWND hParentWnd,CPaintManagerUI* pManager = NULL);


		void SetImage(CDuiString pImage);
		void SetBorder(RECT rcBorder);
		void SetPrepared(bool bPrepared = true);
		void SetVisible(bool bVisible = true);

	protected:
		static LRESULT CALLBACK ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void Update(HWND hParent);

		void Show(HWND hParent); 

	public:
		static std::map<HWND, CUIShadow *> s_Shadowmap;
		static HINSTANCE s_hInstance;
		HWND m_hParentWnd;
	private:
		CDuiImage* m_pMemImage;
		// ��ӰͼƬ
		CDuiString m_szImage;
		// UI������
		CPaintManagerUI* m_pManager;
		// �Ƿ�ˢ��
		bool m_bUpdate;
		// �Ƿ�ɼ�
		bool m_bVisible;
		// �Ƿ�����
		enum ShadowStatus
		{
			SS_ENABLED = 1,	// Shadow is enabled, if not, the following one is always false
			SS_VISABLE = 1 << 1,	// Shadow window is visible
			SS_PARENTVISIBLE = 1<< 2,	// Parent window is visible, if not, the above one is always false
			SS_DISABLEDBYAERO = 1 << 3	// Shadow is enabled, but do not show because areo is enabled
		};
		BYTE m_Status;
		// �Ƿ�Ͷ�������Ϣ
		bool m_bTrans;
		// ��Ӱ��С
		RECT m_rcBorder;
		// ƫ����
		RECT m_rcOffset;
		// ��ʼλ��
		POINT m_ptPos;
		// ���ڴ�С
		LPARAM m_WndSize;
		// ���ھ��
		HWND m_hWnd;
		// ��������Ϣ������
		LONG m_OriParentProc;
		// �������Ƿ񲼾����
		bool m_bPrepared;
	};
}

#endif //__UISHADOW_H__
