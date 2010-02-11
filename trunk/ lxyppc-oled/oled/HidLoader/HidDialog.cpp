// HidDialog.cpp : implementation file
//

#include "stdafx.h"
#include "HidLoader.h"
#include "HidDialog.h"


// CHidDialog dialog

IMPLEMENT_DYNAMIC(CHidDialog, CDialog)

CHidDialog::CHidDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CHidDialog::IDD, pParent)
{

}

CHidDialog::CHidDialog(UINT nIDTemplate, CWnd* pParent /*= NULL*/)
    :CDialog(nIDTemplate, pParent)
{
}

CHidDialog::~CHidDialog()
{
}

void CHidDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHidDialog, CDialog)
END_MESSAGE_MAP()


// CHidDialog message handlers

LRESULT CHidDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: Add your specialized code here and/or call the base class
    if(WM_DEVICECHANGE == message){
        CUsbHid::OnDeviceChange(wParam,lParam);
    }
    return __super::WindowProc(message, wParam, lParam);
}

void CHidDialog::SetTitle(const CString& title)
{
    m_title = title;
}

void CHidDialog::OnConnect(LPCTSTR path)
{
    if(m_activeDevice.CompareNoCase(path) == 0){
        SetWindowText(m_title + _T(" - Connect"));
    }
}

void CHidDialog::OnDisconnect(LPCTSTR path)
{
    if(m_activeDevice.CompareNoCase(path) == 0){
        SetWindowText(m_title + _T(" - Disconnect"));
    }
}

LONG CHidDialog::Open(LPCTSTR path)
{
    LONG res = CUsbHid::Open(path);
    if(res == ERROR_SUCCESS){
        m_activeDevice = path;
    }
    return res;
}

LONG CHidDialog::Close()
{
    LONG res = CUsbHid::Close();
    if(res == ERROR_SUCCESS){
    }
    return res;
}