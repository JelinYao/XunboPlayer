#pragma once
#include "Common\Common.h"
#include <string>
using std::wstring;



class CGlobal
{
public:
	HWND GetMainWnd() const		{ return m_hMainWnd; }
	void SetMainWnd(HWND val)	{ m_hMainWnd = val; }
	long GetVolume()const		{ return m_nVolume; }
	void SetVolume(long nVolume){ m_nVolume = nVolume; }
	const wstring& GetExePath()const	{ return m_strExePath; }
	static CGlobal* Instance()
	{
		static CGlobal obj;
		return &obj;
	}
	void Init()
	{
		m_strExePath = GetRunPathW();
		wstring strIniPath = GetUserDir() + L"Player.ini";
		m_nVolume = GetPrivateProfileInt(L"config", L"volume", 50, strIniPath.c_str());
		if (m_nVolume<0 || m_nVolume>100)
			m_nVolume = 50;
	}
	void UnInit()
	{
		wchar_t szVolume[10];
		if (m_nVolume<0 || m_nVolume>100)
			m_nVolume = 50;
		_itow(m_nVolume, szVolume, 10);
		wstring strIniPath = GetUserDir() + L"Player.ini";
		WritePrivateProfileString(L"config", L"volume", szVolume, strIniPath.c_str());
	}
	wstring GetUserDir()
	{
		wchar_t szPath[MAX_PATH] = { 0 };
		//CSIDL_APPDATA C:\Users\yaojn\AppData\Roaming
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);
		wstring strRootDir;
		if (FAILED(hr))
		{
			strRootDir = m_strExePath + L"XunboYun\\";
		}
		else
		{
			strRootDir.append(szPath);
			strRootDir.append(L"\\XunboYun\\");
		}
		SHCreateDirectory(NULL, strRootDir.c_str());
		return strRootDir;
	}

protected:
	CGlobal()
		: m_hMainWnd(NULL)
		, m_nVolume(50)
	{
	}
	~CGlobal()
	{
	}

private:
	HWND m_hMainWnd;
	wstring m_strExePath;
	long m_nVolume;
};