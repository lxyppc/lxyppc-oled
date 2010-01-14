// ScreenSimulationDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "ColorConvertor.h"

// CScreenSimulationDlg dialog
class CScreenSimulationDlg : public CDialog
{
// Construction
public:
	CScreenSimulationDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SCREENSIMULATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CStatic m_virtualScreen;
public:
    // Reset the items' position
    void OnResetItems(void);
public:
    BOOL DisplayData(const void* pData, long width, long height, long scale, ColorFormat colorFormat);
public:
    afx_msg void OnBnClickedTest();
public:
    void*   m_pDisplayBuffer;
    LONG    m_virtualCx;
    LONG    m_virtualCy;
};
