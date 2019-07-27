#pragma once
#include <string>
using std::wstring;
using std::string;
typedef unsigned char byte;

HANDLE	GetTokenByPid( DWORD dwPid );

BOOL	IsRunasAdmin( HANDLE hProcess );

string	U2A(const wstring& str);

wstring A2U(const string& str);

string	U2Utf(const wstring& wstrUnicode) ;

wstring Utf2U(const string &str);

wstring GetRunPathW();

wstring GetPCName();

bool	CreateGuid(OUT wstring& strGuid);

wstring	StrLowerW(const wstring& str);
string	StrLowerA(const string& str);

wstring StrUpperW(const wstring& str);
string  StrUpperA(const string& str);

bool	RegAddBootRun(const wstring& strCmd);
bool	RegDelBootRun();

void	MyParseUrlW(const wstring& strUrl, wstring& strHostName, wstring& strPage, WORD& sPort);

void	MyParseUrlA(const string& strUrl, string& strHostName, string& strPage, WORD& sPort);

wstring SizeToString(__int64 nSize);

wstring GetThunderPath();

wstring DateToString(__int64 nTime);

bool	GetMagnetHash(const wstring& strMagnet, OUT wstring& strHash);

bool	GetFileExtName(const wstring& strName, OUT wstring& strExtName);

wstring GetFileType(const wstring& strExtName);

bool	CopyToClipboard(const wstring& strData);