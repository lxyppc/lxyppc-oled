/* Includes ------------------------------------------------------------------*/
#define VAR_DEF
#include "includes.h"

u32     readPtr;          /// read pointer of the recieve ring buffer
vu32    writePtr;         /// write pointer of the recieve ring buffer
u8      recvBuffer[RING_SIZE][RECV_BUF_SIZE];
//u8      sendBusy;
u8  USERHID_StringVersion[USERHID_SIZ_STRING_VERSION] = 
{
    USERHID_SIZ_STRING_VERSION,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,             /* bDescriptorType */
};
//u32         reportNow;

/* Replace the callback function for bulk in endpoint */

LDR_STATUS  WaitAndSendUsbData(u8* pData, u16 size, u8 bForce)
{
  if(pData == NULL){
    return _GetEPTxStatus(ENDP1) == EP_TX_VALID ? LDR_NOT_READY : LDR_SUCCESS;
  }
  if(_GetEPTxStatus(ENDP1) != EP_TX_VALID || bForce){
    UserToPMABufferCopy(pData, _GetEPTxAddr(ENDP1), size);
    SetEPTxCount(ENDP1, size);
    SetEPTxValid(ENDP1);
    return LDR_SUCCESS;
  }else{
    return LDR_NOT_READY;
  }
}

LDR_STATUS   ReadUsbData(u8* pData, u16 size)
{
  u8* res = NULL;
  if(readPtr == writePtr){
    return LDR_NOT_READY;
  }
  if(size > RECV_BUF_SIZE){
    return LDR_NOT_SUPPORT;
  }
  if(pData == NULL){
    return LDR_NOT_SUPPORT;
  }
  res = recvBuffer[readPtr & RING_MASK];
  for(u32 i=0;i<RECV_BUF_SIZE;i++){
    pData[i] = res[i];
  }
  readPtr++;
  return LDR_SUCCESS;
}

inline u32 SystemCall(u32 p0, u32 p1, u32 p2, u32 p3)
{
  return  ((u32(*)(u32,u32,u32,u32))__SVC)(p0,p1,p2,p3);
}

IrqFunc_t   RegisterIrq(int IRQChannel, IrqFunc_t newFunc)
{
  int irqNum = 16 + IRQChannel;
  switch(irqNum){
  case 11:  //  SVCHandler
  case 14:  //  PendSVC
  case USB_HP_CAN_TX_IRQChannel+16:
  case USB_LP_CAN_RX0_IRQChannel+16:
    return 0;
  }
  if(irqNum<0 || irqNum> DMA2_Channel4_5_IRQChannel+16){
    return 0;
  }
  u32 old = ((vu32*)NVIC_VectTab_RAM)[irqNum];
  ((vu32*)NVIC_VectTab_RAM)[irqNum] = (u32)newFunc;
  return (IrqFunc_t)old;
}

#define   SVC_NUMBER      4
void* SVCDisplatchBuf[SVC_NUMBER] = {
  (void*)WaitAndSendUsbData,
  (void*)ReadUsbData,
  (void*)RegisterIrq,
  (void*)USB_Init,
};
/*******************************************************************************
* Function Name  : SVCHandler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  SVCDispatch(PTaskContext stack)
{
  if(stack->R0 < SVC_NUMBER ){
    stack->R0 = //SystemFunction(stack->R0, stack->R1, stack->R2, stack->R3);
    ((u32(*)(u32,u32,u32))SVCDisplatchBuf[stack->R0])(stack->R1, stack->R2, stack->R3);
  }else{
    stack->R0 = LDR_NOT_SUPPORT;
  }
}
void  SVCHandler(void)
{
  asm("MRS r0, MSP");
  ((void(*)(void))SVCDispatch)();
}

DeviceFeature   deviceFeature;
void  ProgApp(void);
static  WorkStatus  activeWorkMode = WS_UNKNOWN;
static  u32       bootStatck = 0;
/*******************************************************************************
* Function Name  : PendSVC
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u32  PendSVCHandler(PTaskContext stack)
{
  volatile WorkStatus mode = (WorkStatus)(deviceFeature.workMode.value);
  PTaskContext newStack = NULL;
  if(mode == WS_USER && activeWorkMode != WS_USER){
    if (((*(vu32*)AppBaseAddr) & 0x2FFE0000 ) == 0x20000000){
      u32 JumpAddress = *(vu32*) (AppBaseAddr + 0x4);
      newStack = (PTaskContext) (*(vu32*) AppBaseAddr);
      newStack->xPSR = stack->xPSR;
      newStack->LR = stack->LR;
      newStack->EntryPoint = (u32)JumpAddress;
      activeWorkMode = WS_USER;
    }else{
      mode = WS_BOOT;
    }
  }
  
  if(mode == WS_BOOT && activeWorkMode != WS_BOOT){
    newStack = (PTaskContext)bootStatck;
    newStack->xPSR = stack->xPSR;
    newStack->LR = stack->LR;
    newStack->EntryPoint = (u32)ProgApp;
    activeWorkMode = WS_BOOT;
  }
  deviceFeature.workMode.value = activeWorkMode;
  if(newStack){
    return (u32)newStack;
  }
  return (u32)stack;
}

void PendSVC(void)
{
  asm("MRS r0, MSP");
  asm("add r0,r0,#0x04");
  ((void(*)(void))PendSVCHandler)();
  asm("pop {lr}");
  asm("MSR MSP, r0");
  asm("push {lr}");
}

int main(void)
{
  for(u32 i=0;i<96;i++){
    ((vu32*)NVIC_VectTab_RAM)[i] = ((vu32*)NVIC_VectTab_FLASH)[i];
  }
  //SetInterrupFunction(TIM2_IRQChannel,TIM2_IRQHandler);
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x00);
  
  BootloaderInterruptConfig();
  
  deviceFeature.ID = 0x01;
  deviceFeature.flashSize = *((vu16*)0x1FFFF7E0);
  deviceFeature.ramSize = 0;
  deviceFeature.workMode.value = WS_USER;
  deviceFeature.workMode.ID = 0x02;
  activeWorkMode = WS_UNKNOWN;
  vu32 result = SystemCall(0,0,64,0);
  result++;
//  NVIC_SetSystemHandlerPendingBit(SystemHandler_PSV);
  //result++;
  bootStatck = __MRS_MSP();
  NVIC_SetSystemHandlerPendingBit(SystemHandler_PSV);
  //InvokeBootloader(switchReason);
  while(1);
}


#ifdef  DEBUG 
/*******************************************************************************
* Function Name  : assert_failedd
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

