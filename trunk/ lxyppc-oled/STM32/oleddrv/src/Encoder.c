/*******************************************************************************
* File Name          : Encoder.c
* Author             : lxyppc
* Version            : V1.0
* Date               : 10-01-29
* Description        : Main program body
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "Encoder.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define   ENCODER_TIM             TIM4
/*
    0000: No filter, sampling is done at
    0001: fSAMPLING=fCK_INT, N=2.
    0010: fSAMPLING=fCK_INT, N=4.
    0011: fSAMPLING=fCK_INT, N=8.
    0100: fSAMPLING=fDTS/2, N=6.
    0101: fSAMPLING=fDTS/2, N=8.
    0110: fSAMPLING=fDTS/4, N=6.
    0111: fSAMPLING=fDTS/4, N=8.
    1000: fSAMPLING=fDTS/8, N=6.
    1001: fSAMPLING=fDTS/8, N=8.
    1010: fSAMPLING=fDTS/16, N=5.
    1011: fSAMPLING=fDTS/16, N=6.
    1100: fSAMPLING=fDTS/16, N=8.
    1101: fSAMPLING=fDTS/32, N=5.
    1110: fSAMPLING=fDTS/32, N=6.
    1111: fSAMPLING=fDTS/32, N=8.
*/
// Tim clock division is 1, so the FDTS = 72MHz
// Fsample = FDTS/4 = 18MHz, and sample 6 times
#define   ENC_FILTER              6
#define   MAX_COUNT               1200
#define   ENCODER_TIM_PERIOD      (MAX_COUNT*3)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static    u8    keyStatus = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Enc_Init
* Description    : Initialize the encoder
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  Enc_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef  TIM_ICInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable SPI1 and GPIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
  
  // PA.6,PA.7 for encoder, PA.0 for the encoder button
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  
      /* Timer configuration in Encoder mode */
  TIM_DeInit(ENCODER_TIM);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 0;  // No prescaling 
  TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD-1;  
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(ENCODER_TIM, &TIM_TimeBaseStructure);
  
  TIM_EncoderInterfaceConfig(ENCODER_TIM, TIM_EncoderMode_TI12, 
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  TIM_ICStructInit(&TIM_ICInitStructure);
  
  TIM_ICInitStructure.TIM_ICFilter = ENC_FILTER;//ICx_FILTER;
  TIM_ICInit(ENCODER_TIM, &TIM_ICInitStructure);
  
  // Initial interuppt structures
//  NVIC_InitStructure.NVIC_IRQChannel = TIM3_UP_IRQChannel;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;
//  NVIC_Init(&NVIC_InitStructure);
//  
//  // Clear all pending interrupts
//  TIM_ClearFlag(ENCODER_TIM, TIM_FLAG_Update);
//  TIM_ITConfig(ENCODER_TIM, TIM_IT_Update, ENABLE);
 
  ENCODER_TIM->CNT = 0;
  TIM_Cmd(ENCODER_TIM, ENABLE);
}

/*******************************************************************************
* Function Name  : Enc_GetCount
* Description    : Get current encoder count
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
s16   Enc_GetCount(void)
{
  static  u16   lastCount = 0;
  u16  curCount = ENCODER_TIM->CNT;
  s32 dAngle = curCount - lastCount;
  if(dAngle >= MAX_COUNT){
    dAngle -= ENCODER_TIM_PERIOD;
  }else if(dAngle < -MAX_COUNT){
    dAngle += ENCODER_TIM_PERIOD;
  }
  lastCount = curCount;
  {
    static u8 lastDown = 0;
    if(GPIOA->IDR & GPIO_Pin_0){
      if(lastDown>=2){
        lastDown = 0;
        keyStatus = 1;
      }else{
        lastDown = 0;
      }
    }else{
      if(lastDown&0x40){
      }else{
        lastDown++;
      }
    }
  }
  return (s16)dAngle;
}

/*******************************************************************************
* Function Name  : Enc_IsKeyDown
* Description    : Is the encoder button down
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8    Enc_IsKeyDown(void)
{
  if(keyStatus){
    keyStatus = 0;
    return 1;
  }
  return 0;
}
