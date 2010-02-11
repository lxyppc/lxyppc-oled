// HidLoaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HidLoader.h"
#include "HidLoaderDlg.h"

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


// CHidLoaderDlg dialog




CHidLoaderDlg::CHidLoaderDlg(CWnd* pParent /*=NULL*/)
	: CHidDialog(CHidLoaderDlg::IDD, pParent)
    , m_fileValid(FALSE)
    , m_bDeviceOpen(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_curDeviceFeature.workMode.value = WS_UNKNOWN;
}

void CHidLoaderDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_HEX_VIEW, m_hexView);
    DDX_Control(pDX, IDC_FILE_PATH, m_hexPath);
    DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}

BEGIN_MESSAGE_MAP(CHidLoaderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_LOAD_FILE, &CHidLoaderDlg::OnBnClickedLoadFile)
    ON_BN_CLICKED(IDC_UPDATE_FW, &CHidLoaderDlg::OnBnClickedUpdateFw)
END_MESSAGE_MAP()


// CHidLoaderDlg message handlers

BOOL CHidLoaderDlg::OnInitDialog()
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
    SetTitle(_T("HidLoader"));
    Monitor(0,0,GetSafeHwnd(),LXYPPC_USAGE_PAGE,LXYPPC_USAGE);
    vector<CHidDevice>  devs;
    ::EnumHidDevice(0x0,0x0,devs,LXYPPC_USAGE_PAGE,LXYPPC_USAGE);
    if(devs.size()){
        OnConnect(devs[0].m_path.c_str());
    }
    m_hexView.SetDataSize(1024);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHidLoaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHidLoaderDlg::OnPaint()
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
HCURSOR CHidLoaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHidLoaderDlg::OnDropFiles(HDROP hDropInfo)
{
    // TODO: Add your message handler code here and/or call default
	int count = ::DragQueryFileA(hDropInfo,0xFFFFFFFF,NULL,MAX_PATH);
	if(count<1) goto __Fail;
	TCHAR Buffer[MAX_PATH] = {0};
    if(::DragQueryFile(hDropInfo,0,Buffer,MAX_PATH)>0){
        size_t len = _tcslen(Buffer);
        if( (Buffer[len-1] == L'x' || Buffer[len-1] == L'X')
            && (Buffer[len-2] == L'e' || Buffer[len-2] == L'E')
            && (Buffer[len-3] == L'h' || Buffer[len-3] == L'H')
            && (Buffer[len-4] == L'.' || Buffer[len-4] == L'.')
            ){
                m_fileValid = LoadFile(CString(Buffer));
        }
    }
__Fail:
	__super::OnDropFiles(hDropInfo);
}

void CHidLoaderDlg::OnBnClickedLoadFile()
{
    // TODO: Add your control notification handler code here
    TCHAR szFilters[]=
        _T("Hex Files (*.hex)|*.hex|All Files (*.*)|*.*||");
    CFileDialog	fileDlg(true,_T("*"),_T("*.hex"),OFN_FILEMUSTEXIST| OFN_HIDEREADONLY|OFN_EXPLORER  ,szFilters,0,0);
    if( fileDlg.DoModal ()==IDOK ){
        m_fileValid = LoadFile(fileDlg.GetPathName());
    }
}

BOOL CHidLoaderDlg::LoadFile(const CString & path)
{
    BOOL res = m_romImage.LoadHexFile(path);
    if(res){
        m_hexView.SetDataSize(m_romImage.size());
        m_hexView.SetData(m_romImage.data(),m_romImage.size(),m_romImage.startAddress);
        m_hexPath.SetWindowText(path);
    }
    return res;
}

unsigned long CrcGen_STM32(unsigned long data[], unsigned long size)
{
  unsigned long crc = 0xFFFFFFFF;
  for(unsigned long i=0;i<size;i++){
    unsigned long wdata = data[i];
    for(unsigned long j=0;j<32;j++){
      if( (crc ^ wdata) & 0x80000000 ){
        crc = 0x04C11DB7 ^ (crc<<1);
      }else{
        crc <<=1;
      }
      wdata<<=1;
    }
  }
  return crc;
}

BOOL CROMImage::LoadHexFile(LPCTSTR filename)
{
    try{
		CHexFile	hex(filename);
        if(!hex.LineCount){
            LOG((LOG_OUT,"Fail to load hex file!\n"));
            return FALSE;
        }
        memset(flash,0xff,flashMaxSize);
        DWORD	linearAddress = 0;
        DWORD	lastFlashAddr = 0;
        DWORD	startFlashAddr = flashBaseAddress + flashMaxSize;
        for(int i =0; i<hex.LineCount;i++){
            CHexFileLine& line = hex.Lines[i];
            switch(line.RecordType){
            case t_Linear:
                linearAddress = 
                    (((DWORD)line.Records[0])<<24) + 
                    (((DWORD)line.Records[1])<<16);
                break;
            case t_Data:
                if(line.RecordAddress + linearAddress >= flashBaseAddress){
                    DWORD addr = line.RecordAddress + linearAddress - flashBaseAddress;
                    if(addr<flashMaxSize){
						memcpy(&flash[addr],
						line.Records,
						line.RecordNumber );
						if(lastFlashAddr < addr+line.RecordNumber-1+flashBaseAddress){
							lastFlashAddr = addr+line.RecordNumber-1+flashBaseAddress;
						}
                        if(startFlashAddr > addr + flashBaseAddress && line.RecordNumber){
                            startFlashAddr = addr + flashBaseAddress;
                        }
					}
                }
                break;
            default:
                break;
            }
        }

        startAddress = startFlashAddr;
        endAddress = lastFlashAddr;
        crc32 = CrcGen_STM32(
		(unsigned long*)&flash[startAddress - flashBaseAddress],
		size()/4);
        return TRUE;
    }
    catch(...){
        return FALSE;
    }
    return TRUE;
}

void CHidLoaderDlg::OnConnect(LPCTSTR path)
{
    if(!m_bDeviceOpen){
        if(ERROR_SUCCESS == Open(path)){
            m_bDeviceOpen = TRUE;
            UpdateFeature();
        }
    }
    __super::OnConnect(path);
}

void CHidLoaderDlg::OnDisconnect(LPCTSTR path)
{
    if(CString(path).CompareNoCase(CUsbHid::m_devicePath) == 0){
        Close();
        m_bDeviceOpen = FALSE;
    }
    __super::OnDisconnect(path);
}

void CHidLoaderDlg::UpdateFeature(void)
{
    m_curDeviceFeature.ID = 0x0001;
    GetFeature(&m_curDeviceFeature,sizeof(m_curDeviceFeature),0);
    CString s;
    s.Format(_T("Flash: %d KBytes"),m_curDeviceFeature.flashSize);
    GetDlgItem(IDC_FLASH_SIZE)->SetWindowText(s);
    s.Format(_T("RAM: %d KBytes"),m_curDeviceFeature.ramSize);
    GetDlgItem(IDC_RAM_SIZE)->SetWindowText(s);
    switch(m_curDeviceFeature.workMode.value){
        case WS_BOOT:
            s = _T("Bootloader Ready!");
            break;
        case WS_USER:
            s = _T("User App Ready!");
            break;
        default:
            s = _T("Work Mode Unknown!");
            break;
    }
    GetDlgItem(IDC_WORK_MODE)->SetWindowText(s);
}

void CHidLoaderDlg::OnBnClickedUpdateFw()
{
    // TODO: Add your control notification handler code here
    if(!m_bDeviceOpen){
        return;
    }
    if(!m_fileValid){
        return;
    }
	DWORD id;
	HANDLE hThread = CreateThread(NULL,0,ProgramCallback,this,0,&id);
	if(hThread){
		CloseHandle(hThread);
	}
}

BOOL CHidLoaderDlg::EnterBootMode(void)
{
    if(m_curDeviceFeature.workMode.value != WS_BOOT){
        BYTE buf[2] = {0x02, WS_BOOT};
        if(SetFeature(buf,2,0) == ERROR_SUCCESS){
            Sleep(50);
            UpdateFeature();
            if(m_curDeviceFeature.workMode.value != WS_BOOT){
                return FALSE;
            }else{
                return TRUE;
            }
        }else{
            return FALSE;
        }
    }
    return TRUE;
}

DWORD CHidLoaderDlg::Program(void)
{
    if(!EnterBootMode()){
        return -1;
    }
    ResetRxBuffer();

    USB_OUT_DATA data;
    data.HeadData.ID = 0x01;
    data.HeadData.action = BLC_WRITE;
    data.HeadData.header.appCrc32 = m_romImage.crc32;
    data.HeadData.header.appSize = m_romImage.size();
    data.HeadData.header.appStartAddr = m_romImage.startAddress;
    if(ERROR_SUCCESS != Write(&data,64,0)){
        return -1;
    }
    int romSize = m_romImage.size();
    m_progress.SetRange32(0,romSize);
    for(int i=0; i<romSize; i+=60){
        data.ProgData.ID = 0x01;
        data.ProgData.action = BLC_ROM;
        int len = 60;
        if(romSize-i<60){
            len = romSize-i;
        }
        memset(data.ProgData.blockClip,0xff,60);
        memcpy(data.ProgData.blockClip, m_romImage.data()+i,len);
        if(ERROR_SUCCESS != Write(&data,64,0)){
            return -1;
        }
        m_progress.SetPos(i);
    }

    return 0;
}

void CHidLoaderDlg::ResetRxBuffer(void)
{
	if(!m_hFile)return;
	BYTE buf[128];
	HidD_GetIndexedString(m_hFile,RESET_EP1RX,buf,128);
}
