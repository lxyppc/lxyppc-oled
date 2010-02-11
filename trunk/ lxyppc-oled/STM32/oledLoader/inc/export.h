///////////////////////////////////////////////////////////////////////
///        This header is used by the user applications
///    Date:      2010-02-10
///    Company:   lxyppc
///    Author:    lxyppc
///    Rev:       0.01
///    E-mail:    lxyppc@163.com
///////////////////////////////////////////////////////////////////////
///
///  Interface 0 is used for bootloader control and simple data transfer
///  It report descriptor can not be modified by the user application
///  It use the endpoint 1
///
///  Interface 1 is set as keyboard protocol in boot protocol
///  it report descriptor can be modified, it use the endpoint 2
///
///  Interface 2 is set as mouse protocol in boot protocol
///  it report descriptor can be modified, it use the endpoint 3
///
///////////////////////////////////////////////////////////////////////

#ifndef   EXPORT_H
#define   EXPORT_H
/////////////////////////////////////
//// Export type defines
/////////////////////////////////////

/////////////////////////////////////////////////////////////////////
/// type define for the loader return value
typedef   int   LDR_STATUS;

//////////////////////////////////////////////////////////////////////
/// reture valus of the bootloader functions
/// for more information see the functions below 
///
#define   LDR_SUCCESS           0
#define   LDR_OVER_TIME         1
#define   LDR_OUT_OF_RANGE      2
#define   LDR_NOT_SUPPORT       3
#define   LDR_NOT_READY         4


typedef   void(*IrqFunc_t)(void);

/////////////////////////////////////////////////////////////////////////////////
/// Send data to USB host, if pipe is busy, wait for 0.5s
/// @Parameter: 
///      pData:  a pointer to the data to be send
///      size:   size of the data to be send
///      epNum:  endpoint number to send this data
/// @Return value:
///     When pData is NULL:     If ep is busy, return LDR_NOT_READY
///                             If ep is not busy, return LDR_SUCCESS
///     When pData is NOT NULL: If ep is not busy, return LDR_SUCCESS
///                             If ep is busy, return LDR_NOT_READY
///
#ifdef    BOOTLOADER
LDR_STATUS  WaitAndSendUsbData(u8* pData, u16 size, u8 bForce);

LDR_STATUS  ReadUsbData(u8* pData, u16 size);

IrqFunc_t   RegisterIrq(int IRQChannel, IrqFunc_t newFunc);
#else   //#ifdef    BOOTLOADER
#define     WaitAndSendUsbData(pData, size, bForce)\
  ((LDR_STATUS(*)(u32,u32,u32,u32))__SVC)(0,(u32)pData),(u32)size,(u32)bForce)

#define     ReadUsbData(pData, size)\
  ((LDR_STATUS(*)(u32,u32,u32,u32))__SVC)(1,(u32)pData),(u32)size,0)

#define     RegisterIrq(IRQChannel, newFunc)\
  ((IrqFunc_t(*)(u32,u32,u32,u32))__SVC)(2,(u32)IRQChannel,(u32)newFunc,0)

#define     InitUSB()\
  ((u32(*)(u32,u32,u32,u32))__SVC)(3,0,0,0)

#endif  //#ifdef    BOOTLOADER

#endif // #ifndef   EXPORT_H

