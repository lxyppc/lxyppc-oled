///////////////////////////////////////////////////////////////////////////
///    This file is also used by the Win32 application
///////////////////////////////////////////////////////////////////////////
#ifndef BOOTLOADER_H
#define BOOTLOADER_H
/// When device serial is not specified
/// use the chip's unique ID with prefix L"CP"
#define   FLASH_BASE    0x08000000
#define   RAM_BASE      0x20000000

//// Usage page and usage for the bootloader interface
#define   LXYPPC_USAGE_PAGE         0x97
#define   LXYPPC_USAGE              0x01
#define   LXYPPC_VID                0x1985
#define   LXYPPC_PID                0x1017

typedef struct _DataHeader
{
  unsigned long   appStartAddr;   /// >0x08003800
  unsigned long   appSize;        /// application size in DWORD
  unsigned long   dummy;//appBlockSize;   /// block size
  unsigned long   appCrc32;       /// crc value
}DataHeader,*PDataHeader;

/// USB output data strcuture for OUT_PGM
typedef union _USB_OUT_DATA
{
  unsigned char   RawData[64];
  struct{
    unsigned char  ID;          // Report ID, must be 0x01
    unsigned char  action;      // BLC_WRITE for prgram
    unsigned char  noUss[14];
    unsigned long  dummy1[4];
    DataHeader     header;
    unsigned char  noUse2[16];
  }HeadData;

  struct{
    unsigned char  ID;          // Report ID, must be 0x01
    unsigned char  action;      // BLC_ROM for prgram
    unsigned char  noUse[2];
    unsigned long  blockClip[15];
  }ProgData;
  
}USB_OUT_DATA, *PUSB_OUT_DATA;

/// USB output data structure for BLC_READ
typedef struct _DATA_BLC_READ{
  unsigned char id;   // fixed 0x01
  unsigned char majorCode;
  unsigned char minorCode;
  unsigned char dummy1;
  unsigned long dummy2[3];
  unsigned long dummy3[4];
  unsigned long startAddr;      /// Start addr
  unsigned long length;         /// Read length in DWORD
  unsigned long dummy4[6];
}DATA_BLC_READ, *PDATA_BLC_READ;

// Device feature
typedef struct  _DeviceFeature
{
  unsigned short ID;          // fixed 0x0001
  unsigned short flashSize;   // Flash size in KBytes
  unsigned short ramSize;     // RAM size in KBytes
  struct{
    unsigned char ID;     // fixed 0x02
    unsigned char value;  // Current work mode
  }workMode;
  //unsigned short workMode;    // current work mode
}DeviceFeature;

// Device work mode
typedef enum
{
  WS_BOOT,
  WS_USER,
  WS_UNKNOWN,
}WorkStatus;

/// Bootloader control code, follow the ID OUT_LOADER code
#define   BLC_SWITCH    0x01
#define   BLC_READ      0x02
#define   BLC_WRITE     0x03
#define   BLC_ROM       0x04

/// 
#define   IN_DEFAULT        0x01
#define   OUT_DEFAULT       0x01

/// Bootloader report code, follow the ID IN_LOADER code
#define   BLR_READ_OK        0x01
#define   BLR_READ_FAIL      0x02
#define   BLR_PROG_OK        0x03
#define   BLR_PROG_FAIL      0x04


//// User application state
#define     UAS_OK              0x01
#define     UAS_ERROR           0x02
#define     UAS_CRASH           0x03
#define     UAS_EMPTY           0x04

#define     DebugParamAddr        0x08003000
#define     UserAppState()        (*(vu32*)0x08003000)

#define     AppBaseAddr           0x08003000
#define     MinProgAddr           0x08003000
#define     MinReadAddr           0x08003000

//////////////////////////////////////////////////////////////////////
/// Magic number for RESET the EP1 out endpoint, GetIndexedString(0x1A)
#define   RESET_EP1RX   0x1A

#endif

