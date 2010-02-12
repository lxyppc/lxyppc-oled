#include "includes.h"

#define     FLASH_WAIT_TIMEOUT      100000

void CopyData(u32* des, u32* src,u32 size){
  while(size){
    size--;
    *des = *src;
    des++;
    src++;
  }
}
int  WriteDevice(PUSB_OUT_DATA pData);
int  ReadDevice(PDATA_BLC_READ pData);

void  ProgApp(void)
{
  ConnectUSB();
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);
  
#if DEBUG_BOARD
  u32 i=0;
#endif
  u8 data[64];
  //SetDeviceWorkMode(DWM_PROGRAM);
  while(1){
#if DEBUG_BOARD
    i++;
    if(i>1000000){
      i=0;
      if(Is_LD3_ON()){
        LD3_OFF();
      }else{
        LD3_ON();
      }
    }
#endif
    if(readPtr != writePtr){
      u32 reason = UAS_ERROR;
      u8* res = recvBuffer[readPtr & RING_MASK];
      readPtr++;
      switch(res[1]){
      case BLC_WRITE:
        FLASH_Unlock();
        reason = WriteDevice((PUSB_OUT_DATA)res);
        FLASH_Lock();
        data[0] = IN_DEFAULT;
        if(reason == UAS_ERROR){
          data[1] = BLR_PROG_FAIL;
        }else{
          data[1] = BLR_PROG_OK;
        }
        while( WaitAndSendUsbData(0,64,0)!= LDR_SUCCESS ){}
        WaitAndSendUsbData(data,64,1);
        break;
      case BLC_READ:
        ReadDevice((PDATA_BLC_READ)res);
        break;
      default:
        break;
      }
    }
  }
}

extern DeviceFeature   deviceFeature;

int  WriteDevice(PUSB_OUT_DATA pData)
{
  DataHeader  header;
  u32         curLen = 0;
  u32         reason = UAS_ERROR;
  u32         bufferValid;
  u32         startMem;
  /* Check the header data */
  CopyData((u32*)&header,(u32*)(&pData->HeadData.header),4);
  u32 flashSize = (u32)deviceFeature.flashSize*1024;
  if(header.appStartAddr < MinProgAddr || (header.appStartAddr >= (FLASH_BASE+flashSize)) ){
    header.appStartAddr = MinProgAddr;
    return reason;
  }
  if( header.appSize > flashSize ){
    header.appSize = flashSize;
    return reason;;
  }
  if( (header.appStartAddr + header.appSize) >= (FLASH_BASE+flashSize)){
    return reason;;
  }
  
  u32 pageSize = deviceFeature.flashSize<256 ? 1024 : 2048 ;
  if(header.appStartAddr & (pageSize-1)){
    header.appStartAddr &= (pageSize-1);
    return reason;;
  }
  
  for(u32 i=0;i<header.appSize;i+=pageSize){
    if(FLASH_WaitForLastOperation(FLASH_WAIT_TIMEOUT)!=FLASH_TIMEOUT){
      FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
    }
    FLASH_ErasePage(header.appStartAddr+i);
  }
  if(header.appSize == 0){
    reason = UAS_EMPTY;
    return reason;
  }

  while(1){
    if(readPtr == writePtr){
      continue;
    }
    
    u8* res = recvBuffer[readPtr & RING_MASK];
    bufferValid = (writePtr ^ readPtr) & RING_MASK ; /// zero means buffer overflow
    if(res[1] != BLC_ROM){
      readPtr++;
      reason = UAS_ERROR;
      break;
    }
    readPtr++;
    
    for(u32 i= 0; i<15; i++){
      if(FLASH_WaitForLastOperation(FLASH_WAIT_TIMEOUT)!=FLASH_TIMEOUT){
        FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
      }
      if(curLen){
        FLASH_ProgramWord(header.appStartAddr + curLen,((PUSB_OUT_DATA)res)->ProgData.blockClip[i]);
      }else{
        startMem = ((PUSB_OUT_DATA)res)->ProgData.blockClip[i];
      }
      curLen+=4;
    }
    if(curLen>=header.appSize){
      if(FLASH_WaitForLastOperation(FLASH_WAIT_TIMEOUT)!=FLASH_TIMEOUT){
        FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
      }
      CRC_ResetDR();
      CRC_CalcCRC(startMem);
      if( header.appCrc32 == CRC_CalcBlockCRC((u32*)(header.appStartAddr+4),(header.appSize-4)/4)){
        reason = UAS_OK;
        break;
      }else{
        reason = UAS_ERROR;
        break;
      }
    }
    /// The buffer is locked, now release it
    if(!bufferValid){
      SetEPRxStatus(ENDP1, EP_RX_VALID);
    }
  }
  if(reason == UAS_OK){
    FLASH_ProgramWord(header.appStartAddr,startMem);
    if(FLASH_WaitForLastOperation(FLASH_WAIT_TIMEOUT)!=FLASH_TIMEOUT){
      FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
    }
    reason = *(vu32*)header.appStartAddr == startMem ? UAS_OK : UAS_ERROR;
  }
  return reason;
}


int  ReadDevice(PDATA_BLC_READ pData)
{
  u32   addr;
  u32   length;
  u32   curLen = 0;
  u8    data[64] = {IN_DEFAULT,BLR_READ_OK};
  u32*  pBlock = (u32*)(data + 4);
  addr = pData->startAddr;
  length = pData->length;
  
  if( (addr<MinReadAddr) || (length > 0x3400) ){
    data[1] = BLR_READ_FAIL;
    while( WaitAndSendUsbData(0,64,0)!= LDR_SUCCESS ){}
    WaitAndSendUsbData(data,64,1);
    return 0;
  }
  //aesEncInit();
  for(u32 i=0;i<length;i+=4){
    pBlock[curLen] = *(vu32*)addr;
    curLen++;
    addr+=4;
    if(curLen == 15){
      curLen = 0;
      while( WaitAndSendUsbData(0,64,0)!= LDR_SUCCESS ){}
      WaitAndSendUsbData(data,64,1);
    }
  }
  if(curLen){
    while( WaitAndSendUsbData(0,64,0)!= LDR_SUCCESS ){}
    WaitAndSendUsbData(data,64,1);
  }
  return 0;
}
