// ScreenSimulationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScreenSimulation.h"
#include "ScreenSimulationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CScreenSimulationDlg dialog




CScreenSimulationDlg::CScreenSimulationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScreenSimulationDlg::IDD, pParent)
    ,m_pDisplayBuffer(NULL)
    ,m_virtualCx(0)
    ,m_virtualCy(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScreenSimulationDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_VIR_SCREEN, m_virtualScreen);
}

BEGIN_MESSAGE_MAP(CScreenSimulationDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_TEST, &CScreenSimulationDlg::OnBnClickedTest)
END_MESSAGE_MAP()


// CScreenSimulationDlg message handlers

BOOL CScreenSimulationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    OnResetItems();
    m_pDisplayBuffer = new DWORD[m_virtualCx*m_virtualCy];
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScreenSimulationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CScreenSimulationDlg::OnPaint()
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
        CDC* pDC = m_virtualScreen.GetDC();
        CBitmap bmp;
        BOOL res = bmp.CreateBitmap(m_virtualCx,m_virtualCy,1,32,m_pDisplayBuffer);
        CDC memDC;
        memDC.CreateCompatibleDC(pDC);
        memDC.SelectObject(&bmp);
        pDC->BitBlt(0,0,m_virtualCx,m_virtualCy,&memDC,0,0,SRCCOPY);
        m_virtualScreen.ReleaseDC(pDC);
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CScreenSimulationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


#define     SetItemPosition(id,x,y,cx,cy)   \
    GetDlgItem(id)->SetWindowPos(NULL,(x),(y),(cx),(cy),SWP_NOZORDER)
// Reset the items' position
void CScreenSimulationDlg::OnResetItems(void)
{
    if(!GetDlgItem(IDC_VIR_SCREEN)->GetSafeHwnd())return;

    RECT rect;
    GetClientRect(&rect);
    LONG w = rect.right - rect.left;
    LONG h = rect.bottom - rect.top;
    LONG margin = 10;
    LONG x = margin;
    LONG y = margin;
    SetItemPosition(IDC_VIR_SCREEN, x,y,w-margin*2,h-100);
}

BOOL CScreenSimulationDlg::DisplayData(const void* pData, long width, long height, long scale, ColorFormat colorFormat)
{

    if(!m_pDisplayBuffer){
        m_virtualCx = width*scale;
        m_virtualCy = height*scale;
        m_pDisplayBuffer = new DWORD[m_virtualCx*m_virtualCy];
    }else if(m_virtualCx*m_virtualCy < width*height*scale*scale){
        delete [] m_pDisplayBuffer;
        m_virtualCx = width*scale;
        m_virtualCy = height*scale;
        m_pDisplayBuffer = new DWORD[m_virtualCx*m_virtualCy];
    }
    ConvertImage(m_pDisplayBuffer,pData,m_virtualCx*m_virtualCy*sizeof(DWORD),width,height,scale,CF_RGB888,colorFormat);
    LONG margin = 10;
    SetItemPosition(IDC_VIR_SCREEN, margin,margin,m_virtualCx,m_virtualCy);
    CDC* pDC = m_virtualScreen.GetDC();
    CBitmap bmp;
    BOOL res = bmp.CreateBitmap(m_virtualCx,m_virtualCy,1,32,m_pDisplayBuffer);
    CDC memDC;
    memDC.CreateCompatibleDC(pDC);
    memDC.SelectObject(&bmp);
    pDC->BitBlt(0,0,m_virtualCx,m_virtualCy,&memDC,0,0,SRCCOPY);
    m_virtualScreen.ReleaseDC(pDC);
    return TRUE;
}

void CScreenSimulationDlg::OnBnClickedTest()
{
    // TODO: Add your control notification handler code here
    BYTE buf[1024] = {0xFF,0,0xFF,0,0xFF,0,0xFF,0,
                    0,0x80,0,0x80,0,0x80,0,0x80,
                    0x40,0,0x40,0,0x40,0,0x40,0,
                    0,0xC0,0,0xC0,0,0xC0,0,0xC0,};
    memset(buf+32,0xff,1024-32);
    DisplayData(buf,128,64,4,CF_GRAY1);
}
