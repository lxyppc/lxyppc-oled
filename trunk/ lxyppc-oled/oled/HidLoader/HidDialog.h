#pragma once


// CHidDialog dialog
#include "..\Usb\Inc\UsbHid.h"
class CHidDialog : public CDialog, public CUsbHid
{
	DECLARE_DYNAMIC(CHidDialog)

public:
	CHidDialog(CWnd* pParent = NULL);   // standard constructor
    CHidDialog(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor
	virtual ~CHidDialog();

// Dialog Data
	enum { IDD = IDD_HIDDIALOG };

    void SetTitle(const CString& title);
    virtual void OnConnect(LPCTSTR path);
    virtual void OnDisconnect(LPCTSTR path);
    LONG    Open(LPCTSTR path);
    LONG    Close();
    CString m_title;
    CString m_activeDevice;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
