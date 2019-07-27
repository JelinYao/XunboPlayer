#pragma once


enum PlayerState
{
	Ps_Opening = 0,
	Ps_Pausing,
	Ps_Paused,
	Ps_Playing,
	Ps_Play,
	Ps_Closing,
	Ps_Finish,	//播放完成
	Ps_Begin,	//准备就绪
	Ps_Error,	//播放失败
	Ps_Loading,	//缓冲中
};


class IPlayerCallback
{
public:
	virtual void OnOpenComplete(void* lpUserParam, long lDuiration) = 0;
	virtual void OnStateChange(void* lpUserParam, PlayerState state) = 0;
	virtual void OnDownloadCodec(void* lpUserParam, char* pCodecPath) = 0;
	virtual void OnVolumeChange(void* lpUserParam, long nVolume) = 0;
	virtual void OnSeekCompleted(void* lpUserParam, long nPosition) = 0;
	virtual void OnBuffer(void* lpUserParam, long nPercent) = 0;
	virtual LRESULT OnMsg(void* lpUserParam, HWND hWnd, LONG nMessage, LONG wParam, LONG lParam) = 0;
};