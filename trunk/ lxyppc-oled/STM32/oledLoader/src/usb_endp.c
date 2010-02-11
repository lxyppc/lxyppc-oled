/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_endp.c
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : Endpoint routines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "stm32f10x_lib.h"
#include "config.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "bootloader.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
//u8  dataTest[64];
//void  SendBuffer2USB();
void EP1_OUT_Callback(void)
{
  //static  u8  recvCnt = 0xFF;
  u8 *Receive_Buffer = recvBuffer[writePtr&RING_MASK];
  PMAToUserBufferCopy(Receive_Buffer, ENDP1_RXADDR, RECV_BUF_SIZE);
  
  switch(Receive_Buffer[1]){
    case BLC_ROM:       /// Data used for program the device
      writePtr++;
      if( (writePtr ^ readPtr) & RING_MASK ){
        _SetEPRxStatus(ENDP1, EP_RX_VALID);
      }
      break;
    default:
      writePtr++;
      _SetEPRxStatus(ENDP1, EP_RX_VALID);
      break;
  }
  
  //Receive_Buffer[0] = Receive_Buffer[1];
  //Receive_Buffer[0] = 0x12;
//  UserToPMABufferCopy(Receive_Buffer, ENDP1_TXADDR, 64);
//  SetEPTxCount(ENDP1, 0x40);
//  SetEPTxValid(ENDP1);
  return;
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

