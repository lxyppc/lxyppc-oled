/*******************************************************************************
* File Name          : ClockSet.c
* Author             : lxyppc
* Version            : V1.0
* Date               : 10-01-21
* Description        : Settting of the system clock
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "ClockSet.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
ErrorStatus  HSE_Config(void);
ErrorStatus  HSI_Config(void);


/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();
  HSI_Config();
}

/*******************************************************************************
* Function Name  : HSE_Config
* Description    : Configures the HSE as system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
ErrorStatus  HSE_Config(void)
{
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  ErrorStatus HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  return HSEStartUpStatus;
}

/*******************************************************************************
* Function Name  : HSI_Config
* Description    : Configures the HSI as system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
ErrorStatus  HSI_Config(void)
{
  /* Enable HSI */
  RCC_HSICmd(ENABLE);
  
  /* Wait till HSI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET){
  }
  
  /* Enable Prefetch Buffer */
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

  /* Flash 2 wait state */
  FLASH_SetLatency(FLASH_Latency_2);
 
  /* HCLK = SYSCLK */
  RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
  /* PCLK2 = HCLK */
  RCC_PCLK2Config(RCC_HCLK_Div1); 

  /* PCLK1 = HCLK/2 */
  RCC_PCLK1Config(RCC_HCLK_Div2);

  /* PLLCLK = 8MHz * 9 = 72 MHz */
  RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);

  /* Enable PLL */ 
  RCC_PLLCmd(ENABLE);

  /* Wait till PLL is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  }

  /* Select PLL as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  /* Wait till PLL is used as system clock source */
  while(RCC_GetSYSCLKSource() != 0x08)
  {
  }
  return SUCCESS;
}

unsigned char  SwitchToHSE(void)
{
  ErrorStatus err = SUCCESS;
  
  /* Enable HSI */
  RCC_HSICmd(ENABLE);
  
  /* Wait till HSI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET){
  }
  
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  /* Wait till HSI is used as system clock source */
  while(RCC_GetSYSCLKSource() != 0x00)
  {
  }
  
  /* Enable PLL */ 
  RCC_PLLCmd(DISABLE);
  
  err = HSE_Config();
  if(err != SUCCESS){
    RCC_PLLCmd(DISABLE);
    HSI_Config();
  }
  return err == SUCCESS ? 1 : 0;
}


void    CheckConnection(void)
{
  static u8 bConnect = 0;
  if(GPIOA->IDR & GPIO_Pin_12){
    // Pull up means usb connected
    if(!bConnect){
      bConnect = 1;
      SwitchToHSE();
    }
  }else{
    if(bConnect){
      bConnect = 0;
      SwitchToHSI();
    }
  }
}

unsigned char  SwitchToHSI(void)
{
    /* Enable HSI */
  RCC_HSICmd(ENABLE);
  
  /* Wait till HSI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET){
  }
  
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  /* Wait till HSI is used as system clock source */
  while(RCC_GetSYSCLKSource() != 0x00)
  {
  }
  
  /* Enable PLL */ 
  RCC_PLLCmd(DISABLE);
  
  return HSI_Config() == SUCCESS ? 1 : 0;
}

/*******************************************************************************
* Function Name  : Initial_Tim2
* Description    : Output a specify text at specify position
* Input          : Initialize the timer2 for test the system freqency
* Output         : None
* Return         : End x position for the last character
*******************************************************************************/
#define   ENCODER_TIM1    TIM3
#define   ENC_FILTER    6     
#define   ENCODER_TIM_PERIOD      3600
void Initial_Tim2(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_ICInitTypeDef  TIM_ICInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable SPI1 and GPIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  
  // PA.8,PA.9 for encoder1, PA.0, PA.1 for encoder2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
  
      /* Timer configuration in Encoder mode */
  TIM_DeInit(ENCODER_TIM1);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 0;  // No prescaling 
  TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD-1;  
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(ENCODER_TIM1, &TIM_TimeBaseStructure);
  
  TIM_EncoderInterfaceConfig(ENCODER_TIM1, TIM_EncoderMode_TI12, 
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  TIM_ICStructInit(&TIM_ICInitStructure);
  
  TIM_ICInitStructure.TIM_ICFilter = ENC_FILTER;//ICx_FILTER;
  TIM_ICInit(ENCODER_TIM1, &TIM_ICInitStructure);
  
  // Initial interuppt structures
//  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQChannel;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ENCODER_PRE_EMPTION_PRIORITY;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
//  NVIC_Init(&NVIC_InitStructure);
//  
//  // Clear all pending interrupts
//  TIM_ClearFlag(ENCODER_TIM1, TIM_FLAG_Update);
//  TIM_ITConfig(ENCODER_TIM1, TIM_IT_Update, ENABLE);
 
  ENCODER_TIM1->CNT = 0;
  TIM_Cmd(ENCODER_TIM1, ENABLE);
  
//  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//  
//  TIM_DeInit(TIM2);
//  TIM_TimeBaseStructure.TIM_Prescaler = 0;
//  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//  TIM_TimeBaseStructure.TIM_Period = 1;
//  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
//  TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
//  
//  TIM_OCInitStructure.TIM_Pulse = 1;
//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Disable;
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
//  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
//  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
//  TIM_OC2Init(TIM2,&TIM_OCInitStructure);
//  
//  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
//  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
//  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
//  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
//  TIM_ICInitStructure.TIM_ICFilter = 6;
//  TIM_ICInit(TIM2,&TIM_ICInitStructure);
//  
//  TIM_CtrlPWMOutputs(TIM2, ENABLE);
//  TIM_Cmd(TIM2,ENABLE);
}
