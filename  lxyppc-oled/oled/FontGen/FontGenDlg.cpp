// FontGenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FontGen.h"
#include "FontGenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFontGenDlg dialog




CFontGenDlg::CFontGenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFontGenDlg::IDD, pParent)
    , m_chStart(_T("A"))
    , m_chEnd(_T("Z"))
    , m_valStart('A')
    , m_valEnd('Z')
    , m_bWidth(TRUE)
    , m_bHeight(TRUE)
    , m_sentence(_T(""))
    , m_bVertical(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFontGenDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PRE_VIEW, m_preView);
    DDX_Control(pDX, IDC_EDIT1, m_editPreview);
    DDX_Text(pDX, IDC_EDIT5, m_chStart);
    DDX_Text(pDX, IDC_EDIT6, m_chEnd);
    DDX_Text(pDX, IDC_EDIT3, m_valStart);
    DDX_Text(pDX, IDC_EDIT4, m_valEnd);
    DDX_Check(pDX, IDC_WIDTH, m_bWidth);
    DDX_Check(pDX, IDC_Height, m_bHeight);
    DDX_Control(pDX, IDC_EDIT2, m_codeOutput);
    DDX_Text(pDX, IDC_EDIT7, m_sentence);
    DDX_Check(pDX, IDC_VERTICAL, m_bVertical);
}

BEGIN_MESSAGE_MAP(CFontGenDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_CHANGE_FONT, &CFontGenDlg::OnBnClickedChangeFont)
    ON_EN_CHANGE(IDC_EDIT5, &CFontGenDlg::OnEnChangeEdit5)
    ON_EN_CHANGE(IDC_EDIT6, &CFontGenDlg::OnEnChangeEdit6)
    ON_EN_CHANGE(IDC_EDIT3, &CFontGenDlg::OnEnChangeEdit3)
    ON_EN_CHANGE(IDC_EDIT4, &CFontGenDlg::OnEnChangeEdit4)
    ON_BN_CLICKED(IDC_GENERATE, &CFontGenDlg::OnBnClickedGenerate)
    ON_BN_CLICKED(IDC_GENERATE_SPEC, &CFontGenDlg::OnBnClickedGenerateSpec)
END_MESSAGE_MAP()


// CFontGenDlg message handlers

BOOL CFontGenDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    m_editPreview.SetWindowText(_T("Sample"));
    memset(&m_chFormat,0,sizeof(CHARFORMAT));
    m_chFormat.cbSize = sizeof(CHARFORMAT);
    m_chFormat.dwMask = 0xFFFFFFFF;//CFM_COLOR | CFM_FACE;
    m_chFormat.crTextColor = RGB(0,0,0);
    _tcscpy(m_chFormat.szFaceName, _T("Fixedsys"));
    CFontDialog ft(m_chFormat);
    ft.GetCurrentFont(&m_logFont);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFontGenDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFontGenDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFontGenDlg::OnBnClickedChangeFont()
{
    // TODO: Add your control notification handler code here
    m_chFormat.dwMask = 0xFFFFFFFF;
    CFontDialog fontDlg(m_chFormat);
    //CFontDialog fontDlg;
    CFont* pFont = NULL;
    if(fontDlg.DoModal() == IDOK){
        fontDlg.GetCurrentFont(&m_logFont);
        m_chFormat.dwMask = 0xFFFFFFFF;
        fontDlg.GetCharFormat(m_chFormat);
        UpdatePreview();
    }
}

void CFontGenDlg::OnEnChangeEdit5()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    UpdateData();
    TCHAR ch = m_chStart[0];
    m_valStart = ch;
    UpdateData(FALSE);
}

void CFontGenDlg::OnEnChangeEdit6()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    UpdateData();
    TCHAR ch = m_chEnd[0];
    m_valEnd = ch;
    UpdateData(FALSE);
}

void CFontGenDlg::OnEnChangeEdit3()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    UpdateData();
    TCHAR ch = m_valStart;
    m_chStart = ch;
    UpdateData(FALSE);
}

void CFontGenDlg::OnEnChangeEdit4()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    UpdateData();
    TCHAR ch = m_valEnd;
    m_chEnd = ch;
    UpdateData(FALSE);
}


#define   SPACE         _T("                   ")
void    CFontGenDlg::GenerateFontCode(BOOL bSpecifySentence)
{
    CString output;
    output += _T("    /* Char  Code  ");
    if(m_bWidth){
        output += _T("width, ");
    }
    if(m_bHeight){
        output += _T("height, ");
    }

    output += _T(" {data}  */\r\n");

    CDC* pDC = m_preView.GetDC();
    CFont   font;
    font.CreateFontIndirect(&m_logFont);
    CFont* pFont = pDC->SelectObject(&font);

    int dataMax = 0;
    UINT start = m_valStart;
    UINT end = m_valEnd;
    if(bSpecifySentence){
        start = 0;
        end = m_sentence.GetLength();
        if(end)end--;
    }

    for(UINT i = start; i<=end; i++){
        BYTE data[32][32] = {0};
        TCHAR ch;
        if(bSpecifySentence){
            ch = m_sentence.GetAt(i);
        }else{
            ch = i;
        }
        CString s;
        s = ch;
        CSize size = 
        pDC->GetTextExtent(s);
        //TEXTMETRIC metric;
        //pDC->GetTextMetrics(&metric);

        CString line;
        line.Format(_T("    {/* %s  0x%04X*/  "),s,i);
        CString tmp;
        if(m_bWidth){
            tmp.Format(_T("%6d,"),size.cx);
            line += tmp;
        }
        if(m_bHeight){
            tmp.Format(_T("%6d,"),size.cy);
            line += tmp;
        }
        line += _T(" {\r\n");

        pDC->TextOut(0,0,s);

        if(m_bVertical){
            for(int col = 0; col < size.cx; col++){
                for(int row = 0; row < size.cy; row++){
                    if(pDC->GetPixel(col,row) == RGB(0,0,0)){
                        data[col][row/8] |= (1<<(row%8));
                    }
                }
            }
        }else{
            for(int col = 0; col < size.cx; col++){
                for(int row = 0; row < size.cy; row++){
                    if(pDC->GetPixel(col,row) == RGB(0,0,0)){
                        data[row][col/8] |= (1<<(col%8));
                    }
                }
            }
        }
        int cnt = 0;
        CString rowRes(SPACE);
        if(m_bVertical){
            for(int row =0; row< (size.cy+7)/8; row++){
                for(int col = 0; col < size.cx; col++){
                    CString ts;
                    ts.Format(_T("0x%02X,"),data[col][row]);
                    rowRes += ts;
                    cnt++;
                    if((cnt&7) == 0){
                        rowRes += _T("\r\n");
                        rowRes += SPACE;
                    }
                }
            }
        }else{
            for(int row = 0; row < size.cy; row++){
                for(int col =0; col< (size.cx+7)/8; col++){
                    CString ts;
                    ts.Format(_T("0x%02X,"),data[row][col]);
                    rowRes += ts;
                    cnt++;
                    if((cnt&7) == 0){
                        rowRes += _T("\r\n");
                        rowRes += SPACE;
                    }
                }
            }
        }

        if(cnt>dataMax){
            dataMax = cnt;
        }
        line += rowRes;
        line+=_T("}},\r\n");
        output+= line;
    }
    output+= _T("};");
    CString head(_T("typedef struct _FontData\r\n")
                _T("{\r\n"));
    head += m_bWidth ? _T("    unsigned char width;\r\n") : _T("");
    head += m_bHeight ? _T("    unsigned char height;\r\n") : _T("");
    CString arrSize;
    arrSize.Format(_T("    unsigned char data[%d];\r\n}FontData;\r\n"),dataMax);
    head += arrSize;
    head += _T("static const FontData fontBuffer[] = {\r\n");
    head += output;
    m_codeOutput.SetWindowText(head);

    
    int len = ::WideCharToMultiByte(CP_ACP,0,(LPCTSTR)head,-1,NULL,0,NULL,NULL);
	char *pStr = new char[len+1];
	::WideCharToMultiByte(CP_ACP,0,(LPCTSTR)head,-1,pStr,len+1,NULL,NULL);

    pDC->SelectObject(pFont);
    m_preView.ReleaseDC(pDC);
    if(OpenClipboard()){
        if(EmptyClipboard()){
            HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, len+1);
            memcpy(GlobalLock(hData), pStr, len+1); 
            GlobalUnlock(hData);
            if (::SetClipboardData(CF_TEXT, hData) == NULL){
                AfxMessageBox(_T("Unable to set Clipboard data")); 
            }
        }
        CloseClipboard();
    }

    delete [] pStr;
}

void CFontGenDlg::OnBnClickedGenerate()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    UpdatePreview();
    GenerateFontCode(FALSE);
}

void CFontGenDlg::UpdatePreview(void)
{
    CFont   font;
    font.CreateFontIndirect(&m_logFont);
    CDC* pDC = m_preView.GetDC();
    RECT rect;
    m_preView.GetWindowRect(&rect);
    pDC->Rectangle(0,0,rect.right-rect.left,rect.bottom-rect.top);
    CFont* pFont = pDC->SelectObject(&font);
    CString s;
    m_editPreview.GetWindowText(s);
    pDC->TextOut(0,0,s);
    pDC->SelectObject(pFont);
    m_preView.ReleaseDC(pDC);
}

void CFontGenDlg::OnBnClickedGenerateSpec()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    UpdatePreview();
    GenerateFontCode(TRUE);
}
