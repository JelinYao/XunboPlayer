#include "stdafx.h"
#include "Common.h"
#include <algorithm>
#include <objbase.h>



HANDLE GetTokenByPid( DWORD dwPid )
{
	HANDLE hRetToken  = NULL;
	HANDLE hProcess	  = NULL;
	HANDLE hToken     = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	try 
	{	
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
		if ( NULL == hProcess )
			throw L"";
		if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken)) 
			throw L"";
		pSD = (PSECURITY_DESCRIPTOR) GlobalAlloc(GPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (pSD == NULL) 
			throw L"";
		if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) 
			throw L"";
		if (!SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE)) 
			throw L"";
		SECURITY_ATTRIBUTES sa;
		sa.nLength              = sizeof(sa);
		sa.lpSecurityDescriptor = pSD;
		sa.bInheritHandle       = TRUE;
		if ( !DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, &sa, \
			SecurityImpersonation, TokenPrimary, &hRetToken) ) 
			throw L"";
	} 
	catch(...) 
	{

	}
	if ( hToken )
		CloseHandle(hToken);
	if ( hProcess )
		CloseHandle(hProcess);
	if ( pSD )
		GlobalFree(pSD);
	return hRetToken;
}


BOOL IsRunasAdmin( HANDLE hProcess )
{
	BOOL bElevated = FALSE;  
	HANDLE hToken = NULL;  

	if ( !OpenProcessToken( hProcess, TOKEN_QUERY, &hToken ) )
	{
		return FALSE;
	}
	TOKEN_ELEVATION tokenEle;
	DWORD dwRetLen = 0;  
	// Retrieve token elevation information
	if ( GetTokenInformation( hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen ) )
	{  
		if ( dwRetLen == sizeof(tokenEle) )
		{
			bElevated = tokenEle.TokenIsElevated;  
		}
	} 
	CloseHandle( hToken );  
	return bElevated;  
}

string U2A(const wstring& str)
{
	string strDes;
	if ( str.empty() )
		goto __end;
	int nLen=::WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), NULL, 0, NULL, NULL);
	if ( 0==nLen )
		goto __end;
	char* pBuffer=new char[nLen+1];
	memset(pBuffer, 0, nLen+1);
	::WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), pBuffer, nLen, NULL, NULL);
	pBuffer[nLen]='\0';
	strDes.append(pBuffer);
	delete[] pBuffer;
__end:
	return strDes;
}

wstring A2U(const string& str)
{
	wstring strDes;
	if ( str.empty() )
		goto __end;
	int nLen=::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	if ( 0==nLen )
		goto __end;
	wchar_t* pBuffer=new wchar_t[nLen+1];
	memset(pBuffer, 0, nLen+1);
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), pBuffer, nLen);
	pBuffer[nLen]='\0';
	strDes.append(pBuffer);
	delete[] pBuffer;
__end:
	return strDes;
}

string U2Utf(const wstring& wstrUnicode)  
{  
	string strRet;
	if( wstrUnicode.empty() )
		return strRet;
	int nLen = WideCharToMultiByte(CP_UTF8, 0, wstrUnicode.c_str(), -1, NULL, 0, NULL, NULL);  
	char* pBuffer=new char[nLen+1];
	pBuffer[nLen] = '\0';
	nLen = WideCharToMultiByte(CP_UTF8, 0, wstrUnicode.c_str(), -1, pBuffer, nLen, NULL, NULL); 
	strRet.append(pBuffer);
	delete[] pBuffer;
	return strRet;  
}

wstring Utf2U(const string &str)
{
	int u16Len = ::MultiByteToWideChar(CP_UTF8, NULL,str.c_str(),(int)str.size(), NULL, 0);
	wchar_t* wstrBuf = new wchar_t[u16Len + 1];
	::MultiByteToWideChar(CP_UTF8, NULL, str.c_str(),(int)str.size(), wstrBuf, u16Len);
	wstrBuf[u16Len] = L'\0';
	wstring wStr;
	wStr.assign(wstrBuf, u16Len);
	delete [] wstrBuf;
	return wStr;
}

wstring GetRunPathW()
{
	wchar_t szPath[MAX_PATH] = {0};
	GetModuleFileNameW(NULL, szPath, MAX_PATH);
	wstring strPath(szPath);
	int nPos = strPath.find_last_of(L"\\");
	strPath = strPath.substr(0, nPos+1);
	return strPath;
}

wstring GetPCName()
{
	wstring strName;
	wchar_t szName[MAX_PATH];
	DWORD dwSize = MAX_PATH;
	GetComputerName(szName, &dwSize);
	strName.append(szName);
	dwSize = MAX_PATH;
	GetUserName(szName, &dwSize);
	strName.append(L"/");
	strName.append(szName);
	return strName;
}


bool CreateGuid(OUT wstring& strGuid)
{
	GUID id;
	WCHAR buff[40] = { '\0' };
	CHAR buff2[40] = { '\0' };
	if (FAILED(CoCreateGuid(&id)))
		return false;
	StringFromGUID2(id, buff, 40);
	strGuid.assign(buff);
	strGuid = strGuid.substr(1, strGuid.size() - 2);
	return true;
}

wstring StrLowerW(const wstring& str)
{
	wstring strOut;
	for (size_t i = 0; i < str.size(); ++i)
	{
		strOut += (wchar_t)towlower(str[i]);
	}
	return strOut;
}

string StrLowerA(const string& str)
{
	string strOut;
	for (size_t i = 0; i < str.size(); ++i)
	{
		strOut += (char)tolower(str[i]);
	}
	return strOut;
}

wstring StrUpperW(const wstring& str)
{
	wstring strOut;
	for (size_t i = 0; i < str.size(); ++i)
	{
		strOut += (wchar_t)towupper(str[i]);
	}
	return strOut;
}

string StrUpperA(const string& str)
{
	string strOut;
	for (size_t i = 0; i < str.size(); ++i)
	{
		strOut += (char)toupper(str[i]);
	}
	return strOut;
}

bool RegAddBootRun(const wstring& strCmd)
{
	HKEY hKey;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey))
	{
		return false;
	}
	RegSetValueEx(hKey, L"XunZhibo", 0, REG_SZ, (byte*)strCmd.c_str(), strCmd.size() * 2);
	RegCloseKey(hKey);
	return true;
}

bool RegDelBootRun()
{
	HKEY hKey;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey))
	{
		return false;
	}
	bool bRet = (ERROR_SUCCESS == RegDeleteValue(hKey, L"XunZhibo"));
	RegCloseKey(hKey);
	return bRet;
}

void MyParseUrlW(const wstring& strUrl, wstring& strHostName, wstring& strPage, WORD& sPort)
{
	sPort = 80;
	wstring strTemp = strUrl;
	int nPos = strTemp.find(L"http://");
	if (wstring::npos != nPos)
		strTemp = strTemp.substr(nPos + 7, strTemp.size() - nPos - 7);
	nPos = strTemp.find('/');
	if (wstring::npos == nPos)//没有找到 /
		strHostName = strTemp;
	else
		strHostName = strTemp.substr(0, nPos);
	int nPos1 = strHostName.find(':');
	if (nPos1 != wstring::npos)
	{
		wstring strPort = strTemp.substr(nPos1 + 1, strHostName.size() - nPos1 - 1);
		strHostName = strHostName.substr(0, nPos1);
		sPort = (WORD)_wtoi(strPort.c_str());
	}
	if (wstring::npos == nPos)
		return;
	strPage = strTemp.substr(nPos, strTemp.size() - nPos);
}

void MyParseUrlA(const string& strUrl, string& strHostName, string& strPage, WORD& sPort)
{
	sPort = 80;
	string strTemp = strUrl;
	int nPos = strTemp.find("http://");
	if (string::npos != nPos)
		strTemp = strTemp.substr(nPos + 7, strTemp.size() - nPos - 7);
	nPos = strTemp.find('/');
	if (string::npos == nPos)//没有找到 /
		strHostName = strTemp;
	else
		strHostName = strTemp.substr(0, nPos);
	int nPos1 = strHostName.find(':');
	if (nPos1 != string::npos)
	{
		string strPort = strTemp.substr(nPos1 + 1, strHostName.size() - nPos1 - 1);
		strHostName = strHostName.substr(0, nPos1);
		sPort = (WORD)atoi(strPort.c_str());
	}
	if (string::npos == nPos)
		return;
	strPage = strTemp.substr(nPos, strTemp.size() - nPos);
}
static const float g_G = 1024 * 1024 * 1024.0;
static const float g_M = 1024 * 1024.0;
wstring SizeToString(__int64 nSize)
{
	if (nSize < 1)
		return L"0 KB";
	LPWSTR lpTag = NULL;
	float nValue = 0;
	if (nSize < 1024)
	{
		nValue = (float)nSize;
		lpTag = L"Bit";
	}
	else if (nSize < g_M)
	{
		nValue = nSize/1024.0f;
		lpTag = L"K";
	}
	else if (nSize < g_G)
	{
		nValue = nSize / g_M;
		lpTag = L"M";
	}
	else
	{
		nValue = nSize / g_G;
		lpTag = L"G";
	}
	wchar_t szSize[20] = { 0 };
	swprintf_s(szSize, L"%.2f %s", nValue, lpTag);
	return wstring(szSize);
}

wstring GetThunderPath()
{
	BOOL bIsWow64 = FALSE;
	if (!IsWow64Process(GetCurrentProcess(), &bIsWow64))
		return L"";
	wchar_t* pRegBase = NULL;
	if (bIsWow64)
		pRegBase = L"SOFTWARE\\Wow6432Node\\Thunder Network\\ThunderOem\\thunder_backwnd";
	else
		pRegBase = L"SOFTWARE\\Thunder Network\\ThunderOem\\thunder_backwnd";
	HKEY hKey;
	if (ERROR_SUCCESS != RegOpenKey(HKEY_LOCAL_MACHINE, pRegBase, &hKey))
		return L"";
	byte buffer[MAX_PATH + 1] = { 0 };
	DWORD dwSize = 2048, dwType = 0;
	if (ERROR_SUCCESS != RegQueryValueEx(hKey, L"Path", NULL, &dwType, buffer, &dwSize))
		return L"";
	wstring strPath((wchar_t*)buffer);
	return strPath;
}

wstring DateToString(__int64 nTime)
{
	struct tm newTime;
	time_t time = nTime;
	localtime_s(&newTime, &time);
	wchar_t szTime[64] = { 0 };
	swprintf_s(szTime, L"%d-%02d-%02d %02d:%02d:%02d", 1900 + newTime.tm_year, 1 + newTime.tm_mon, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
	return wstring(szTime);
}

bool GetMagnetHash(const wstring& strMagnet, OUT wstring& strHash)
{
	//magnet:?xt=urn:btih:4bca467c62e1476ea781e041becc9ddbd58f2e20
	wstring strOutHash;
	strHash.clear();
	while (true)
	{
		int nFind = strMagnet.find(L"magnet:?");
		if (nFind == wstring::npos)
			break;
		nFind = strMagnet.find(L"xt=urn:btih:", nFind + 8);
		if (nFind == wstring::npos)
			break;
		int nLast = strMagnet.find(L"&", nFind + 12);
		if (nLast == wstring::npos)
		{
			strOutHash = strMagnet.substr(nFind + 12);
		}
		else
		{
			strOutHash = strMagnet.substr(nFind + 12, nLast - nFind - 12);
		}
		break;
	}
	if (strOutHash.size() == 40)
	{
		strHash = strOutHash;
		return true;
	}
	return false;
}

bool GetFileExtName(const wstring& strName, OUT wstring& strExtName)
{
	if (strName.empty())
		return false;
	int nFind = strName.rfind('.');
	if (nFind == wstring::npos)
		return false;
	strExtName = strName.substr(nFind + 1);
	return true;
}

wstring GetFileType(const wstring& strExtName)
{
	if (_wcsicmp(strExtName.c_str(), L"jpg") == 0 || _wcsicmp(strExtName.c_str(), L"jpeg") == 0 || _wcsicmp(strExtName.c_str(), L"png") == 0
		|| _wcsicmp(strExtName.c_str(), L"gif") == 0)
		return L"图片";
	if (_wcsicmp(strExtName.c_str(), L"mp4") == 0 || _wcsicmp(strExtName.c_str(), L"avi") == 0 || _wcsicmp(strExtName.c_str(), L"rmvb") == 0
		|| _wcsicmp(strExtName.c_str(), L"mkv") == 0 || _wcsicmp(strExtName.c_str(), L"flv") == 0 || _wcsicmp(strExtName.c_str(), L"wmv") == 0
		|| _wcsicmp(strExtName.c_str(), L"mpg") == 0)
		return L"视频";
	if (_wcsicmp(strExtName.c_str(), L"txt") == 0)
		return L"文档";
	if (_wcsicmp(strExtName.c_str(), L"html") == 0 || _wcsicmp(strExtName.c_str(), L"htm") == 0 || _wcsicmp(strExtName.c_str(), L"mht") == 0
		|| _wcsicmp(strExtName.c_str(), L"url") == 0)
		return L"链接";
	if (_wcsicmp(strExtName.c_str(), L"rar") == 0 || _wcsicmp(strExtName.c_str(), L"zip") == 0 || _wcsicmp(strExtName.c_str(), L"7z") == 0)
		return L"压缩包";
	if (_wcsicmp(strExtName.c_str(), L"torrent") == 0)
		return L"种子";
	if (_wcsicmp(strExtName.c_str(), L"mp3") == 0 || _wcsicmp(strExtName.c_str(), L"aac") == 0)
		return L"音频";
	if (_wcsicmp(strExtName.c_str(), L"pdf") == 0 || _wcsicmp(strExtName.c_str(), L"epub") == 0)
		return L"电子书";
	return L"未知";
}


bool CopyToClipboard(const wstring& strData)
{
	if (strData.empty())
		return false;
	if (!OpenClipboard(NULL))
	{
		DWORD dwError = GetLastError();
		return false;//打开剪切板失败
	}
	EmptyClipboard();
	SIZE_T nSize = strData.size() + 1;
	HGLOBAL hClip = GlobalAlloc(GMEM_DDESHARE, nSize * sizeof(wchar_t));
	PTSTR pszBuf = (PTSTR)GlobalLock(hClip);
	wcscpy_s(pszBuf, nSize, strData.c_str());
	GlobalUnlock(hClip);
	SetClipboardData(CF_UNICODETEXT, hClip);
	CloseClipboard();
	return true;
}