/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_vector.c
* Author             : MCD Application Team
* Version            : V2.0.2
* Date               : 07/11/2008
* Description        : STM32F10x vector table for EWARM4.x toolchain.
*                      This module performs:
*                      - Set the initial SP
*                      - Set the initial PC == __program_start,
*                      - Set the vector table entries with the exceptions ISR address,
*                     - Configure external SRAM mounted on STM3210E-EVAL board
*                       to be used as data memory (optional, to be enabled by user)
*                      After Reset the Cortex-M3 processor is in Thread mode,
*                      priority is Privileged, and the Stack is set to Main.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "stm32f10x_it.h"
/* Private typedef -----------------------------------------------------------*/
typedef void( *intfunc )( void );
typedef union { intfunc __fun; void * __ptr; } intvec_elem;

/* Private define ------------------------------------------------------------*/
/* Uncomment the following line if you need to use external SRAM mounted on
   STM3210E-EVAL board as data memory */

/* #define DATA_IN_ExtSRAM */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern  vu32 bDeviceState;

#pragma language=extended
#pragma segment="CSTACK"

void __program_start( void );
#pragma location = "INTVEC"
/* STM32F10x Vector Table entries */
const intvec_elem __vector_table[] =
{
  { .__ptr = __sfe( "CSTACK" ) },
  __program_start,
  SysTickHandler,//NMIException,
  FaultExceptionHandler,//HardFaultException,
  FaultExceptionHandler,//MemManageException,
  FaultExceptionHandler,//BusFaultException,
  FaultExceptionHandler,//UsageFaultException,
  0, 0, 0, 0,            /* Reserved */ 
  SVCHandler,
  SysTickHandler,//DebugMonitor,
  0,                      /* Reserved */
  PendSVC,
  SysTickHandler,
  SysTickHandler,//WWDG_IRQHandler,
  SysTickHandler,//PVD_IRQHandler,
  SysTickHandler,//TAMPER_IRQHandler,
  SysTickHandler,//RTC_IRQHandler,
  SysTickHandler,//FLASH_IRQHandler,
  SysTickHandler,//RCC_IRQHandler,
  SysTickHandler,//EXTI0_IRQHandler,
  SysTickHandler,//EXTI1_IRQHandler,
  SysTickHandler,//EXTI2_IRQHandler,
  SysTickHandler,//EXTI3_IRQHandler,
  SysTickHandler,//EXTI4_IRQHandler,
  SysTickHandler,//DMA1_Channel1_IRQHandler,
  SysTickHandler,//DMA1_Channel2_IRQHandler,
  SysTickHandler,//DMA1_Channel3_IRQHandler,
  SysTickHandler,//DMA1_Channel4_IRQHandler,
  SysTickHandler,//DMA1_Channel5_IRQHandler,
  SysTickHandler,//DMA1_Channel6_IRQHandler,
  SysTickHandler,//DMA1_Channel7_IRQHandler,
  SysTickHandler,//ADC1_2_IRQHandler,
  USB_HP_CAN_TX_IRQHandler,
  USB_LP_CAN_RX0_IRQHandler,
  SysTickHandler,//CAN_RX1_IRQHandler,
  SysTickHandler,//CAN_SCE_IRQHandler,
  SysTickHandler,//EXTI9_5_IRQHandler,
  SysTickHandler,//TIM1_BRK_IRQHandler,
  SysTickHandler,//TIM1_UP_IRQHandler,
  SysTickHandler,//TIM1_TRG_COM_IRQHandler,
  SysTickHandler,//TIM1_CC_IRQHandler,
  SysTickHandler,//TIM2_IRQHandler,
  SysTickHandler,//TIM3_IRQHandler,
  SysTickHandler,//TIM4_IRQHandler,
  SysTickHandler,//I2C1_EV_IRQHandler,
  SysTickHandler,//I2C1_ER_IRQHandler,
  SysTickHandler,//I2C2_EV_IRQHandler,
  SysTickHandler,//I2C2_ER_IRQHandler,
  SysTickHandler,//SPI1_IRQHandler,
  SysTickHandler,//SPI2_IRQHandler,
  SysTickHandler,//USART1_IRQHandler,
  SysTickHandler,//USART2_IRQHandler,
  SysTickHandler,//USART3_IRQHandler,
  SysTickHandler,//EXTI15_10_IRQHandler,
  SysTickHandler,//RTCAlarm_IRQHandler,
  USBWakeUp_IRQHandler,
  SysTickHandler,//TIM8_BRK_IRQHandler,
  SysTickHandler,//TIM8_UP_IRQHandler,
  SysTickHandler,//TIM8_TRG_COM_IRQHandler,
  SysTickHandler,//TIM8_CC_IRQHandler,
  SysTickHandler,//ADC3_IRQHandler,
  SysTickHandler,//FSMC_IRQHandler,
  SysTickHandler,//SDIO_IRQHandler,
  SysTickHandler,//TIM5_IRQHandler,
  SysTickHandler,//SPI3_IRQHandler,
  SysTickHandler,//UART4_IRQHandler,
  SysTickHandler,//UART5_IRQHandler,
  SysTickHandler,//TIM6_IRQHandler,
  SysTickHandler,//TIM7_IRQHandler,
  SysTickHandler,//DMA2_Channel1_IRQHandler,
  SysTickHandler,//DMA2_Channel2_IRQHandler,
  SysTickHandler,//DMA2_Channel3_IRQHandler,
  SysTickHandler,//DMA2_Channel4_5_IRQHandler,
};

