#pragma once
#include "MyPlayer.h"



class CPlayerUI
	:public CControlUI
{
public:
	CPlayerUI();
	~CPlayerUI();
	bool PlayVideo(const wstring& strVideo);//打开文件播放
	void Play();							//播放
	void Pause();							//暂停
	void Stop();							//停止播放
	void SetPlayPosition(long nPos);		//设置播放位置
	long GetPlayPosition();					//获取播放位置
	long GetVolume();						//获取当前音量
	void SetVolume(long lVolume);			//设置音量
	bool IsPlaying();
	bool IsPausd();
	bool IsSeeking();
	long GetBufferProgress();
	void SetPlayLogo(HBITMAP hBmp);
	long GetDuration();						//获取播放时长
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

