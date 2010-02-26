#include "StdAfx.h"
#include "AppSetting.h"

CAppSetting::CAppSetting(void)
{
    TCHAR	IniName[1024];
    GetModuleFileName(NULL,IniName,1024);
	size_t len = _tcslen(IniName);
	IniName[len-1] = _T('i');
	IniName[len-2] = _T('n');
	IniName[len-3] = _T('i');
    m_iniName = IniName;
}

CAppSetting::~CAppSetting(void)
{
}

int CAppSetting::GetIntValue(const xstring& appName, const xstring& keyName, int defValue/* = 0*/)
{
    TCHAR ret[1024] = _T("");
    TCHAR def[256] = _T("");
    _itot(defValue,def,10);
	GetPrivateProfileString(
	  appName.c_str(),
	  keyName.c_str(),
	  def,
	  ret,
	  1024,
	  m_iniName.c_str()
	  );
    return _ttoi(ret);
}

xstring CAppSetting::GetStrValue(const xstring& appName, const xstring& keyName, const xstring& defValue/* = _T("")*/)
{
    TCHAR ret[1024] = _T("");
	GetPrivateProfileString(
	  appName.c_str(),
	  keyName.c_str(),
	  defValue.c_str(),
	  ret,
	  1024,
	  m_iniName.c_str()
	  );
    return xstring(ret);
}

void CAppSetting::SetIntValue(const xstring& appName, const xstring& keyName, int defValue)
{
	TCHAR s[16];
	_itot(defValue,s,10);
	BOOL res;
	res = WritePrivateProfileString(
		appName.c_str(),
		keyName.c_str(),
		s,
		m_iniName.c_str()
		);
}

void CAppSetting::SetStrValue(const xstring& appName, const xstring& keyName, const xstring& defValue)
{
	BOOL res;
	res = WritePrivateProfileString(
		appName.c_str(),
		keyName.c_str(),
		defValue.c_str(),
		m_iniName.c_str()
		);
}


CRet CSetting::operator[](const xstring& key)
{
    return CRet(*this,key);
}

CRet::operator int()
{
    return m_set.m_appSet.GetIntValue(m_set.m_name,m_key);
}

int CRet::operator=(int val)
{
    m_set.m_appSet.SetIntValue(m_set.m_name,m_key,val);
    return m_set.m_appSet.GetIntValue(m_set.m_name,m_key);
}

CRet::operator xstring()
{
    return m_set.m_appSet.GetStrValue(m_set.m_name,m_key);
}

CRet& CRet::operator()(int val){
    if(xstring(*this) == _T("")){
        *this = val;
    }
    return *this;
}

CRet& CRet::operator()(const xstring& val){
    if(xstring(*this) == _T("")){
        *this = val;
    }
    return *this;
}

xstring CRet::operator=(const xstring& val)
{
    m_set.m_appSet.SetStrValue(m_set.m_name,m_key,val);
    return m_set.m_appSet.GetStrValue(m_set.m_name,m_key);
}

xstring		ToXString(LPCWSTR str)
{
#ifdef	UNICODE
	return xstring(str);
#else
	int len = ::WideCharToMultiByte(CP_ACP,0,str,-1,NULL,0,NULL,NULL);
	char *pStr = new char[len+1];
	::WideCharToMultiByte(CP_ACP,0,str,-1,pStr,len+1,NULL,NULL);
	xstring res(pStr);
	delete [] pStr;
	return res;
#endif
}
xstring		ToXString(LPCSTR str)
{
#ifdef	UNICODE
	int len = ::MultiByteToWideChar(CP_ACP,0,str,-1,NULL,0);
	wchar_t *pStr = new wchar_t[len+1];
	::MultiByteToWideChar(CP_ACP,0,str,-1,pStr,len+1);
	/// InitialDevice(pStr);
	xstring res(pStr);
	delete [] pStr;
	return res;
#else
	return xstring(str);
#endif
}

wstring     ToWString(LPCTSTR str)
{
#ifdef	UNICODE
    return wstring(str);
#else
	int len = ::MultiByteToWideChar(CP_ACP,0,str,-1,NULL,0);
	wchar_t *pStr = new wchar_t[len+1];
	::MultiByteToWideChar(CP_ACP,0,str,-1,pStr,len+1);
	/// InitialDevice(pStr);
	wstring res(pStr);
	delete [] pStr;
	return res;
#endif
}

string      ToString(LPCTSTR str)
{
#ifdef	UNICODE
	int len = ::WideCharToMultiByte(CP_ACP,0,str,-1,NULL,0,NULL,NULL);
	char *pStr = new char[len+1];
	::WideCharToMultiByte(CP_ACP,0,str,-1,pStr,len+1,NULL,NULL);
	string res(pStr);
	delete [] pStr;
	return res;
#else
    return string(str);
#endif
}
