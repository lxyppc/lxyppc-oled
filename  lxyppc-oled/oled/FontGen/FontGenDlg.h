// FontGenDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CFontGenDlg dialog
class CFontGenDlg : public CDialog
{
// Construction
public:
	CFontGenDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FONTGEN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CStatic m_preView;
public:
    afx_msg void OnBnClickedChangeFont();
public:
    CHARFORMAT  m_chFormat;
    LOGFONT     m_logFont;
public:
    CEdit m_editPreview;
public:
    CString m_chStart;
public:
    CString m_chEnd;
public:
    UINT m_valStart;
public:
    UINT m_valEnd;
public:
    afx_msg void OnEnChangeEdit5();
public:
    afx_msg void OnEnChangeEdit6();
public:
    afx_msg void OnEnChangeEdit3();
public:
    afx_msg void OnEnChangeEdit4();
public:
    BOOL m_bWidth;
public:
    BOOL m_bHeight;
public:
    afx_msg void OnBnClickedGenerate();
public:
    void UpdatePreview(void);
public:
    CEdit m_codeOutput;
public:
    afx_msg void OnBnClickedGenerateSpec();
public:
    void    GenerateFontCode(BOOL bSpecifySentence);
public:
    CString m_sentence;
public:
    BOOL m_bVertical;
};
