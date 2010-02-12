// HidLoaderDlg.h : header file
//

#pragma once
#include "..\hexedit\hexeditctrl.h"
#include "..\PICData\hexfileline.h"
#include "HidDialog.h"

#pragma pack(push,1)
#include "..\..\STM32\oledLoader\inc\Bootloader.h"
#pragma pack(pop)

#include "afxwin.h"
#include "afxcmn.h"

#ifndef     FLASH_BASE
#define     FLASH_BASE  (0x08000000)
#endif
#ifndef     FLASH_MAX
#define     FLASH_MAX   (512*1024)      // 512KB
#endif

#define		WM_LOADER		WM_USER + 101
#define		WML_DISABLE_ITEMS		1			/// Disable items
#define		WML_ENABLE_ITEMS		2			/// Enable items
#define		WML_UPDATE_STATUS		3			/// Update status

struct  CROMImage
{
    CROMImage():flashBaseAddress(FLASH_BASE),startAddress(0)
    ,endAddress(0),crc32(0xFFFFFFFF),flash(NULL),flashMaxSize(FLASH_MAX){
        flash = new BYTE[FLASH_MAX];
    }
    ~CROMImage(){
        delete [] flash;
    }
    BOOL LoadHexFile(LPCTSTR path);
    BYTE*   flash;
    DWORD   startAddress;
    const DWORD   flashBaseAddress;
    const DWORD   flashMaxSize;
    DWORD   endAddress;
    DWORD   crc32;      // crc32 value
    int     size()const{ return endAddress - startAddress + 1; }
    BYTE*   data(){
        if( startAddress>=flashBaseAddress && size() <= (int)flashMaxSize){
            return flash + (startAddress - flashBaseAddress);
        }
        return flash;
    }
};

#define     LOG_OUT 
#define     LOG(_X_)            

// CHidLoaderDlg dialog
class CHidLoaderDlg : public CHidDialog
{
// Construction
public:
	CHidLoaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_HIDLOADER_DIALOG };

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
    CHexEditor m_hexView;
public:
    afx_msg void OnDropFiles(HDROP hDropInfo);
    virtual void OnConnect(LPCTSTR path);
    virtual void OnDisconnect(LPCTSTR path);
    virtual void OnHidData(void* pData, size_t dataLen);
public:
    afx_msg void OnBnClickedLoadFile();
public:
    BOOL LoadFile(const CString & path);
public:
    BOOL m_fileValid;
    BOOL m_bDeviceOpen;
public:
    CROMImage     m_romImage;
public:
    CEdit m_hexPath;
public:
    DeviceFeature   m_curDeviceFeature;
public:
    void UpdateFeature(void);
public:
    CProgressCtrl m_progress;
public:
    afx_msg void OnBnClickedUpdateFw();
public:
    BOOL EnterBootMode(void);
public:
    DWORD Program(void);
	static DWORD CALLBACK ProgramCallback(LPVOID pVoid){
        ::SendMessage(((CHidLoaderDlg*)pVoid)->m_hParent,WM_LOADER,WML_DISABLE_ITEMS,0);
		DWORD res = ((CHidLoaderDlg*)pVoid)->Program();
        ::SendMessage(((CHidLoaderDlg*)pVoid)->m_hParent,WM_LOADER,WML_ENABLE_ITEMS,res);
		return res;
	}
public:
    void ResetRxBuffer(void);
public:
    HANDLE  m_hEvent;
    BYTE    m_buffer[64];
    CComboBox m_logCombo;
protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
    void EnableItems(bool bEnable);
    afx_msg void OnBnClickedRunApp();
    FILETIME        m_fileTime;
};

