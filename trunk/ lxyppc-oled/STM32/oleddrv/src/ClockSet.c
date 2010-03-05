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
#include "..\..\oledLoader\Inc\Export.h"
#include "bsp.h"

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
  
  DisconnectUSB();
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
  }else{
    ConnectUSB();
  }
  return err == SUCCESS ? 1 : 0;
}


void    CheckConnection(void)
{
  static u8 bConnect = 0;
  if(1){
  //if(IsPGOOD()){
  //if( GPIOD->IDR & GPIO_Pin_3 ){
  //if(GPIOA->IDR & GPIO_Pin_12){
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

