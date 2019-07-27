#ifndef WIN_IRREGULAR_WND_HPP
#define WIN_IRREGULAR_WND_HPP

namespace DuiLib
{

class CIrregularWindow
{
public:

	CIrregularWindow(const HBITMAP hBitmap,DWORD dwAttachWndTransColor = 0x00ff00ff);

	//���Ӵ���
	bool AttachWindow(HWND hWnd);

	void* GetImage(HBITMAP hBitmap,UINT bits = 32);

	//����ͼƬ(ȫ·�� png)
	void SetBackground(const HBITMAP hBitmap);

	//һ����� �ǲ��õ���������� ����������� 
	//�����ӵĴ�����Ӵ��� ��ʾ����ȫ����� ����Ҫ�������ֵ
	void SetAttachWndTransColor(DWORD dwColor);

	HWND GetHandle(){return m_hWnd;}

	void CenterWindow();

protected:

	virtual bool RegisterWindowClass();

	virtual bool Create();

public:

	static void InitGDIplus();

	static void UnInitGDIplus();

private:

	static LRESULT CALLBACK  __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK __HookProc(int nCode,WPARAM wParam,LPARAM lParam);

	static ULONG_PTR gdiplusToken;

	static HHOOK hhk;

protected:

	HWND m_hWnd;

	//�����ӵĴ���ľ��
	HWND m_hAttachWnd;

	DWORD m_dwAttachWndTransColor;

	//����ͼƬ(png)
	HBITMAP m_hBitmap;
};

}

#endif// WIN_IRREGULAR_WND_HPP
