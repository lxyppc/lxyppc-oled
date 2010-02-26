#pragma once

#include <string>
using std::string;
using std::wstring;
#ifdef  _UNICODE
typedef     wstring     xstring;
#else
typedef     string      xstring;
#endif

xstring		ToXString(LPCSTR str);
xstring		ToXString(LPCWSTR str);
wstring     ToWString(LPCTSTR str);
string      ToString(LPCTSTR str);

class CAppSetting;
struct  CRet;

struct  CSetting
{
    CSetting(CAppSetting& appSet, const xstring& name):m_appSet(appSet),m_name(name){}
    CRet operator[](const xstring& key);
    const xstring& m_name;
    CAppSetting& m_appSet;
};

struct  CRet
{
    CRet(CSetting& set, const xstring& key):m_set(set),m_key(key){}
    CRet(const CRet& r):m_val(r.m_val),m_set(r.m_set),m_key(r.m_key){}
    operator int();
    operator xstring();
    xstring operator=(const xstring& val);
    int operator=(int val);
    CRet& operator()(int val);
    CRet& operator()(const xstring& val);
    int m_val;
    CSetting& m_set;
    const xstring& m_key;
};

class CAppSetting
{
public:
    CAppSetting(void);
    CAppSetting(const xstring& iniName):m_iniName(iniName){}
    ~CAppSetting(void);
    int GetIntValue(const xstring& appName, const xstring& keyName, int defValue = 0);
    xstring GetStrValue(const xstring& appName, const xstring& keyName, const xstring& defValue = _T(""));
    void SetIntValue(const xstring& appName, const xstring& keyName, int defValue = 0);
    void SetStrValue(const xstring& appName, const xstring& keyName, const xstring& defValue = _T(""));
    CSetting    operator[](const xstring& appName){ return CSetting(*this,appName); }
protected:
    xstring     m_iniName;
};
