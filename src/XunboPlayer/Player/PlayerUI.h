#pragma once
#include "MyPlayer.h"



class CPlayerUI
	:public CControlUI
{
public:
	CPlayerUI();
	~CPlayerUI();
	bool PlayVideo(const wstring& strVideo);//���ļ�����
	void Play();							//����
	void Pause();							//��ͣ
	void Stop();							//ֹͣ����
	void SetPlayPosition(long nPos);		//���ò���λ��
	long GetPlayPosition();					//��ȡ����λ��
	long GetVolume();						//��ȡ��ǰ����
	void SetVolume(long lVolume);			//��������
	bool IsPlaying();
	bool IsPausd();
	bool IsSeeking();
	long GetBufferProgress();
	void SetPlayLogo(HBITMAP hBmp);
	long GetDuration();						//��ȡ����ʱ��
	int  GetVideoWidth();
	int  GetVideoHeight();
	HWND GetPlayerWnd();
	void SetPlayerCallback(IPlayerCallback* pCallback, void* lpUserParam);

protected:
	virtual void DoInit(); 
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void SetPos(RECT rc);
	virtual void SetInternVisible(bool bVisible /* = true */);

private:
	wstring m_strUrl;
	CMyPlayer m_player;
};

