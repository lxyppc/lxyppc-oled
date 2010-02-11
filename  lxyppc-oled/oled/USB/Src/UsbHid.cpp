#include "StdAfx.h"

#include "..\INC\UsbHid.h"

#include "..\INC\hidsdi++.h"
#include "..\INC\hid.h"
#include "..\INC\setupapi.h"
#include "..\INC\usb100.h"
#include <dbt.h>

#ifndef	LIB_USB_DEFINE
#pragma comment(lib, "..\\USB\\LIB\\setupapi.lib")
#pragma comment(lib, "..\\USB\\LIB\\hid.lib")
#endif

xstring		toXString(LPCWSTR str)
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
xstring		toXString(LPCSTR str)
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


LONG	CUsbHid::Open(LPCTSTR lpDevice)
{
	m_devicePath[0] = 0;
	m_hFile = ::CreateFile(
			lpDevice,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, //&SecurityAttributes,  //no SECURITY_ATTRIBUTES structure
			OPEN_EXISTING,  //No special create flags
			FILE_FLAG_OVERLAPPED, // No special attributes
			NULL);
	if (m_hFile == INVALID_HANDLE_VALUE){
		m_hFile = NULL;
		return (LONG)::GetLastError();
	}
	m_hevtOverlappedWorkerThread = ::CreateEvent(0,true,false,0);
	if (m_hevtOverlappedWorkerThread == 0)
	{
		// Obtain the error information
		m_lLastError = ::GetLastError();
		Close();
		// Return the error
		return m_lLastError;
	}

	m_lLastError = StartListener();
	if (m_lLastError != ERROR_SUCCESS)
		return m_lLastError;
	_tcscpy(m_devicePath,lpDevice);
	return m_lLastError;
}

LONG	CUsbHid::Open(WORD vid, WORD pid, HWND hParent, LPCTSTR filter /*= NULL*/)
{
	m_vid = vid;
	m_pid = pid;
	m_filter = filter;
	m_hParent = hParent;
	RegisterDevice();
	vector<CHidDevice>	devs;
	EnumHidDevice(vid,pid,devs,filter);
	if(devs.size()){
		_tcscpy(m_devicePath,devs[devs.size()-1].m_path.c_str());
		LONG err = Open(devs[devs.size()-1].m_path.c_str());
		if(err == ERROR_SUCCESS){
			if(hParent){
				m_bConnect = TRUE;
			}
		}
		return err;
	}
	return -1;
}

LONG	CUsbHid::Monitor(WORD vid, WORD pid, HWND hParent, LPCTSTR filter /*= NULL*/)
{
	m_vid = vid;
	m_pid = pid;
	m_filter = filter;
	m_hParent = hParent;
	return RegisterDevice();
}

LONG	CUsbHid::Open(WORD vid, WORD pid, HWND hParent, USAGE usagePage, USAGE usage, LPCTSTR filter)
{
	m_usagePage = usagePage;
	m_usage = usage;
	m_vid = vid;
	m_pid = pid;
	m_filter = filter;
	m_hParent = hParent;
	RegisterDevice();
	vector<CHidDevice>	devs;
	EnumHidDevice(vid,pid,devs,usagePage,usage);
	if(devs.size()){
		_tcscpy(m_devicePath,devs[devs.size()-1].m_path.c_str());
		LONG err = Open(devs[devs.size()-1].m_path.c_str());
		if(err == ERROR_SUCCESS){
			if(hParent){
				m_bConnect = TRUE;
			}
		}
		return err;
	}
	return -1;
}

LONG	CUsbHid::Monitor(WORD vid, WORD pid, HWND hParent, USAGE usagePage, USAGE usage, LPCTSTR filter)
{
	m_usagePage = usagePage;
	m_usage = usage;
	return Monitor(vid, pid, hParent, filter);
}

LONG	CUsbHid::Close()
{
	StopListener(1000);
	if(m_hFile){
		::CloseHandle(m_hFile);
		m_hFile = NULL;
	}
	return ERROR_SUCCESS;
}

/// Must write 8 byte every time
LONG	CUsbHid::Write(const void* pData, size_t len, DWORD* realWrite)
{
	/// version is higher or equal than 2.00
	HANDLE hEvent = ::CreateEvent(0,true,false,0);
	OVERLAPPED	ov;
	memset(&ov,0,sizeof(OVERLAPPED));
	ov.hEvent = hEvent;
	DWORD wr;
	BYTE buf[64] = "";
	memcpy(buf,pData,len);
	if(buf[0] == 0x55){
		buf[0] = 0x11;
	}
	BOOL res = ::WriteFile(m_hFile,buf,64,&wr,&ov);
	if (::WaitForSingleObject(hEvent,2000) != WAIT_OBJECT_0){
			// Set the internal error code
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
	return ERROR_SUCCESS;
}

LONG	CUsbHid::Read(void* pData, size_t len, DWORD* realRead)
{
	DWORD ttp;
	if(realRead == NULL){
		realRead = &ttp;
	}
	const BYTE* data = (const BYTE*)pData;
	DWORD wr=0;
	HANDLE hEvent = ::CreateEvent(0,true,false,0);
	OVERLAPPED	ov;
	memset(&ov,0,sizeof(OVERLAPPED));
	ov.hEvent = hEvent;
	BOOL res = ::ReadFile(m_hFile,pData,2,realRead,&ov);
	if (::WaitForSingleObject(hEvent,INFINITE) != WAIT_OBJECT_0)
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();
		return m_lLastError;
	}
	return ERROR_SUCCESS;
}

LONG	CUsbHid::StartListener()
{
	// Check if the watcher thread was already running
	if (m_hListenerThread == 0)
	{
		// Make sure the thread has stopped
		_ASSERTE(!m_fStopping);

		// Start the watcher thread
		DWORD dwThreadId = 0;
		m_hListenerThread = ::CreateThread(0,0,ThreadProc,LPVOID(this),0,&dwThreadId);
		if (m_hListenerThread == 0)
		{
			// Set the error code and exit
			m_lLastError = ::GetLastError();
			return m_lLastError;
		}
	}

	// Return the error
	m_lLastError = ERROR_SUCCESS;
	return m_lLastError;
}

LONG	CUsbHid::StopListener(DWORD dwTimeout)
{
	// Check if the thread is running
	if (m_hListenerThread)
	{
		DWORD code;
		::GetExitCodeThread(m_hListenerThread,&code);
		if(code == STILL_ACTIVE){
			// Set the flag that the thread must be stopped
			m_fStopping = true;
			::SetEvent(m_hevtOverlappedWorkerThread);
			::CancelIo(m_hFile);
			DWORD err = ::GetLastError();
			if(::WaitForSingleObject(m_hListenerThread,dwTimeout) != WAIT_OBJECT_0){
				::TerminateThread(m_hListenerThread,0);
			}

			// The thread has stopped
			m_fStopping = false;
		}

		// Close handle to the thread
		::CloseHandle(m_hListenerThread);
		m_hListenerThread = NULL;
	}

	// Return the error
	m_lLastError = ERROR_SUCCESS;
	return m_lLastError;
}

DWORD WINAPI CUsbHid::ThreadProc (LPVOID lpArg)
{
	return ((CUsbHid*)lpArg)->ThreadProc();
}

DWORD	CUsbHid::ThreadProc (void)
{
	OVERLAPPED	ov;
	memset(&ov,0,sizeof(OVERLAPPED));
	//LPOVERLAPPED lpOverlapped = 0;
	// Keep looping
	do
	{
		DWORD read;
		memset(&ov,0,sizeof(OVERLAPPED));
		ov.hEvent = m_hevtOverlappedWorkerThread;
		BYTE	data[256]={0};
		::ReadFile(m_hFile,data,64,&read,&ov);
		if ((::WaitForSingleObject(m_hevtOverlappedWorkerThread,INFINITE) != WAIT_OBJECT_0))
		{
			// Set the internal error code
			m_lLastError = ::GetLastError();
			return m_lLastError;
		}
		if(m_fStopping){
			break;
		}
		if(ov.InternalHigh > 0){
			OnHidData(data,ov.InternalHigh);
		}else{
			break;
		}
		//if(data[0] == 0x07){
		//	printf("0x%X\n",data[1]);
		//}
	}
	while (!m_fStopping);
	if(!m_fStopping){
		//OnDisconnect(m_devicePath);
	}
	// Bye bye
	return 0;
}


LRESULT	CUsbHid::OnDeviceChange(WPARAM wParam, LPARAM lParam)
{
	TCHAR vid_str[64];
	TCHAR pid_str[64];
	_stprintf(vid_str,_T("VID_%04X"),m_vid);
	_stprintf(pid_str,_T("PID_%04X"),m_pid);
	PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
	PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
	if(DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam){
		if( pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE){
			pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
			LPTSTR p = pDevInf->dbcc_name;
			while(*p){
				if(::islower(*p)){*p -= (_T('a')-_T('A'));}
				p++;
			}
			if(    ( (m_vid == 0) || (_tcsstr(pDevInf->dbcc_name, vid_str) != NULL) )
				&& ( (m_pid == 0) || (_tcsstr(pDevInf->dbcc_name, pid_str) != NULL) )
				&& ( (m_filter == NULL) || (_tcsstr(pDevInf->dbcc_name, m_filter) != NULL) )
				)
			{
					if(DBT_DEVICEARRIVAL == wParam){
						if(CheckUsage(xstring(pDevInf->dbcc_name),m_usagePage,m_usage)){
							OnConnect(pDevInf->dbcc_name);
						}
					}else{
						OnDisconnect(pDevInf->dbcc_name);
					}
			}
		}
	}
	return 0;
}
LRESULT	CUsbHid::RegisterDevice()
{
	if(m_hParent){
		HDEVNOTIFY hDevNotify;
		DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
		ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
		NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		GUID Guid;
		HidD_GetHidGuid(&Guid);
		NotificationFilter.dbcc_classguid = Guid; 
		hDevNotify = RegisterDeviceNotification(m_hParent, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
		if( !hDevNotify ) {
			::MessageBox(NULL,_T("Can't register device notification: "),_T("Error!"),MB_OK);
		}
	}
	return ERROR_SUCCESS;
}


BOOL	CUsbHid::GetAttributes(PHIDD_ATTRIBUTES  Attributes)
{
	if(m_hFile){
		return  HidD_GetAttributes(m_hFile,Attributes);
	}
	return FALSE;
}

BOOL	CUsbHid::GetSerialString(void * p, ULONG len)
{
	if(m_hFile){
		return  HidD_GetSerialNumberString(m_hFile,p,len);
	}
	return FALSE;
}

LONG	CUsbHid::IoControl(void* pData, size_t len, DWORD *realRead)
{
	DWORD read;
	if(realRead == NULL){
		realRead = &read;
	}
	DWORD code = CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_ANY_ACCESS);
	BOOL res = DeviceIoControl(
		this->m_hFile,
		code,
		pData,
		(DWORD)len,
		NULL,
		0,
		realRead,
		NULL
		);
	if(res){
		m_lLastError = ERROR_SUCCESS;
	}else{
		m_lLastError = GetLastError();
	}
	return m_lLastError;
}

LONG	CUsbHid::SetFeature(void* pData, size_t len, DWORD *realRead)
{
	BOOL res = HidD_SetFeature(m_hFile,pData,(ULONG)len);
	return GetLastError();
}

LONG	CUsbHid::GetFeature(void* pData, size_t len, DWORD *realRead)
{
	BOOL res = HidD_GetFeature(m_hFile,pData,(ULONG)len);
	return GetLastError();
}


size_t	EnumHidDevice(WORD vid, WORD pid, vector<CHidDevice>& devs,LPCTSTR lpFilter /*= NULL*/)
{
	devs.clear();
	TCHAR	Product[253];
	string	Prod, String;
	GUID	Guid;
	CHidDevice	temp;
	temp.m_desc = _T("");
	TCHAR vid_str[64];
	TCHAR pid_str[64];

	_stprintf(vid_str,_T("vid_%04x"),vid);
	_stprintf(pid_str,_T("pid_%04x"),pid);

	HidD_GetHidGuid(&Guid);

	HDEVINFO info;
    info=SetupDiGetClassDevs(&Guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    if (info!=INVALID_HANDLE_VALUE)  
	{
		DWORD devIndex = 0;
		SP_INTERFACE_DEVICE_DATA ifData;
		ifData.cbSize=sizeof(ifData);
		//SetupDiEnumDeviceInterfaces(info, NULL, &Guid, devIndex, &ifData);
		//devIndex = ::GetLastError();
		for (devIndex=0;SetupDiEnumDeviceInterfaces(info, NULL, &Guid, devIndex, &ifData);++devIndex)
		{
			DWORD needed;

			SetupDiGetDeviceInterfaceDetail(info, &ifData, NULL, 0, &needed, NULL);

			PSP_INTERFACE_DEVICE_DETAIL_DATA detail=(PSP_INTERFACE_DEVICE_DETAIL_DATA)new BYTE[needed];
			detail->cbSize=sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
			SP_DEVINFO_DATA did={sizeof(SP_DEVINFO_DATA)};
			if (SetupDiGetDeviceInterfaceDetail(info, &ifData, detail, needed, NULL, &did))
			{
				// Add the link to the list of all DFU devices
				if(    ( (vid == 0) || (_tcsstr(detail->DevicePath, vid_str) != NULL) )
					&& ( (pid == 0) || (_tcsstr(detail->DevicePath, pid_str) != NULL) )
					&& ( (lpFilter == NULL) || (_tcsstr(detail->DevicePath, lpFilter) != NULL) )
					)
				{
					temp.m_path = detail->DevicePath;
					if (SetupDiGetDeviceRegistryProperty(info, &did, SPDRP_DEVICEDESC, NULL, (PBYTE)Product, 253, NULL))
						temp.m_desc = Product;
					else
						temp.m_desc = _T("(Unnamed HID device)");
				}
			}

			//if(strstr(detail->DevicePath, vid_str) != NULL
			//	&& ( (pid == 0) || (strstr(detail->DevicePath, pid_str) != NULL) )
			//	)
			//{
			//}

			delete[] (PBYTE)detail;
			if(temp.m_desc != _T("")){
				devs.push_back(temp);
			}
		}
		SetupDiDestroyDeviceInfoList(info);
	}

	return devs.size();
}
BOOL	CheckUsage(xstring &path, USAGE usagePage, USAGE usage)
{
	if(usagePage == 0  && usage == 0 ){
		return TRUE;
	}
	HANDLE file = CreateFile ( path.c_str(),
                            GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL, // no SECURITY_ATTRIBUTES structure
                            OPEN_EXISTING, // No special create flags
                            0, // No special attributes
                            NULL); // No template file
	if(file == INVALID_HANDLE_VALUE ){
		return FALSE;
	}
	BOOL res = TRUE;
	PHIDP_PREPARSED_DATA			Ppd; // The opaque parser info describing this device
    HIDP_CAPS                       Caps; // The Capabilities of this hid device.
	if (!HidD_GetPreparsedData (file, &Ppd))
    {
        res = FALSE;
    }
    if (!HidP_GetCaps (Ppd, &Caps))
    {
        HidD_FreePreparsedData (Ppd);
        res = FALSE;
	}else{
		if(usagePage){
			if(Caps.UsagePage != usagePage){
				res = FALSE;
			}
		}
		if(usage){
			if(Caps.Usage != usage){
				res = FALSE;
			}
		}
	}
	HidD_FreePreparsedData (Ppd);
	::CloseHandle(file);
	return res;
}
size_t	EnumHidDevice(WORD vid, WORD pid, vector<CHidDevice>& devs, USAGE usagePage, USAGE usage)
{
	devs.clear();
	TCHAR	Product[253];
	string	Prod, String;
	GUID	Guid;
	CHidDevice	temp;
	temp.m_desc = _T("");
	TCHAR vid_str[64];
	TCHAR pid_str[64];

	_stprintf(vid_str,_T("vid_%04x"),vid);
	_stprintf(pid_str,_T("pid_%04x"),pid);

	HidD_GetHidGuid(&Guid);

	HDEVINFO info;
    info=SetupDiGetClassDevs(&Guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    if (info!=INVALID_HANDLE_VALUE)  
	{
		DWORD devIndex = 0;
		SP_INTERFACE_DEVICE_DATA ifData;
		ifData.cbSize=sizeof(ifData);
		//SetupDiEnumDeviceInterfaces(info, NULL, &Guid, devIndex, &ifData);
		//devIndex = ::GetLastError();
		for (devIndex=0;SetupDiEnumDeviceInterfaces(info, NULL, &Guid, devIndex, &ifData);++devIndex)
		{
			DWORD needed;

			SetupDiGetDeviceInterfaceDetail(info, &ifData, NULL, 0, &needed, NULL);

			PSP_INTERFACE_DEVICE_DETAIL_DATA detail=(PSP_INTERFACE_DEVICE_DETAIL_DATA)new BYTE[needed];
			detail->cbSize=sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
			SP_DEVINFO_DATA did={sizeof(SP_DEVINFO_DATA)};
			temp.m_desc = _T("");
			if (SetupDiGetDeviceInterfaceDetail(info, &ifData, detail, needed, NULL, &did))
			{
				// Add the link to the list of all DFU devices
				if(    ( (vid == 0) || (_tcsstr(detail->DevicePath, vid_str) != NULL) )
					&& ( (pid == 0) || (_tcsstr(detail->DevicePath, pid_str) != NULL) )
					)
				{
					
					temp.m_path = detail->DevicePath;
					if(CheckUsage(temp.m_path,usagePage,usage)){
						if (SetupDiGetDeviceRegistryProperty(info, &did, SPDRP_DEVICEDESC, NULL, (PBYTE)Product, 253, NULL))
							temp.m_desc = Product;
						else
							temp.m_desc = _T("(Unnamed HID device)");
					}
				}
			}

			//if(strstr(detail->DevicePath, vid_str) != NULL
			//	&& ( (pid == 0) || (strstr(detail->DevicePath, pid_str) != NULL) )
			//	)
			//{
			//}

			delete[] (PBYTE)detail;
			if(temp.m_desc != _T("")){
				devs.push_back(temp);
			}
		}
		SetupDiDestroyDeviceInfoList(info);
	}

	return devs.size();
}
