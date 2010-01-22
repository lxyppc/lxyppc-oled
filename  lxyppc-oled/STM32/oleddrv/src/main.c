/*******************************************************************************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 09-11-21
* Description        : Main program body
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "bsp.h"
#include "SSD1303.h"
#include "Graphics\Graphics.h"  // Graphic primitives layer

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define WAIT_UNTIL_FINISH(x) (x)

/* Private variables ---------------------------------------------------------*/
static const FontData world[2] = {
    /* Char  Code  width, height,  {data}  */
    {/* 世  0x0000*/      16,    16, {
                   0x20,0x20,0x20,0xFE,0x20,0x20,0xFE,0x20,
                   0x20,0x20,0x20,0xFE,0x20,0x20,0x20,0x00,
                   0x00,0x00,0x00,0x3F,0x20,0x20,0x27,0x22,
                   0x22,0x22,0x22,0x27,0x30,0x20,0x00,0x00,
                   }},
    {/* 界  0x0001*/      16,    16, {
                   0x00,0x00,0x00,0xFF,0x49,0x49,0x49,0xFF,
                   0xC9,0x49,0x49,0x49,0xFF,0x00,0x00,0x00,
                   0x00,0x08,0x08,0x84,0x44,0x22,0x1D,0x00,
                   0x00,0xFD,0x02,0x02,0x04,0x0C,0x04,0x00,
                   }},
};

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void SSD1303_IO_Configuration(void);
void SSD1303_Controller_Init(void);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
  SHORT width, height;
  SHORT counter;
#ifdef DEBUG
  debug();
#endif

  /* System Clocks Configuration */
  RCC_Configuration();
  GPIO_AFIODeInit();
  GPIO_DeInit(GPIOB);
  
  /* NVIC configuration */
  NVIC_Configuration();
  
  /* Initialize the SSD1303 related IO */
  SSD1303_IO_Configuration();
  
  /* Initialize the SSD1303 */
  SSD1303_Init();
  
  /* Initialize the SSD1303 controller,
     which is simulated by STM32 DMA and systick*/
  SSD1303_Controller_Init();
  
  Device dev;
  InitialDevice(&dev,&SSD1303_Prop,fontBuffer_fixedsys);
  
  InitGraph();
  
  while(1){
    SetColor(WHITE);
    for(counter=0; counter<GetMaxX(); counter+=20){
      DelayMs(500);
      WAIT_UNTIL_FINISH(Line(counter,0,GetMaxX()-1-counter,GetMaxY()-1));
    }
    
    DelayMs(2000);
    
    for(counter=10; counter<GetMaxY()>>1; counter+=10){
      DelayMs(500);
      WAIT_UNTIL_FINISH(Circle(GetMaxX()>>1,GetMaxY()>>1,counter));
    }
    
    DelayMs(2000);
    WAIT_UNTIL_FINISH(FillCircle(GetMaxX()>>1,GetMaxY()>>1,10));
    
    DelayMs(2000);
    WAIT_UNTIL_FINISH(FillCircle(GetMaxX()>>1,GetMaxY()>>1,20));
    
    DelayMs(2000);
    WAIT_UNTIL_FINISH(FillCircle(GetMaxX()>>1,GetMaxY()>>1,30));
    
    DelayMs(2000);
    SetColor(BLACK);
    ClearDevice();

    DelayMs(2000);
    SetColor(WHITE);
    WAIT_UNTIL_FINISH(Bevel((GetMaxX()>>1)-30,(GetMaxY()>>1)-30,(GetMaxX()>>1)+30,(GetMaxY()>>1)+30,15));
    
    WAIT_UNTIL_FINISH(Bevel((GetMaxX()>>1)-20,(GetMaxY()>>1)-20,(GetMaxX()>>1)+20,(GetMaxY()>>1)+20,15));
    
    WAIT_UNTIL_FINISH(Bevel((GetMaxX()>>1)-10,(GetMaxY()>>1)-10,(GetMaxX()>>1)+10,(GetMaxY()>>1)+10,15));
    
    DelayMs(2000);
    SetColor(BLACK);
    ClearDevice();
    SetColor(WHITE);
    WAIT_UNTIL_FINISH(Arc((GetMaxX()>>1)-30,(GetMaxY()>>1)-30,(GetMaxX()>>1)+30,(GetMaxY()>>1)+30,10,15,0xFF));
    
    WAIT_UNTIL_FINISH(Arc((GetMaxX()>>1)-20,(GetMaxY()>>1)-20,(GetMaxX()>>1)+20,(GetMaxY()>>1)+20,10,15,0xFF));
    
    WAIT_UNTIL_FINISH(Arc((GetMaxX()>>1)-10,(GetMaxY()>>1)-10,(GetMaxX()>>1)+10,(GetMaxY()>>1)+10,10,15,0xFF));
    
    DelayMs(2000);
    SetColor(BLACK);
    ClearDevice();
    
    SetColor(WHITE);
    
    for(counter=0; counter<GetMaxY()>>1; counter+=4){
      DelayMs(500);
      WAIT_UNTIL_FINISH(Rectangle(GetMaxX()/2-counter,
                                  GetMaxY()/2-counter,
                                  GetMaxX()/2+counter,
                                  GetMaxY()/2+counter));
    }
    
    DelayMs(2000);
    SetColor(BLACK);
    ClearDevice();

  }
  
  
  
  
  while(1);
  
  /* Draw a box */
  for(u32 x=0;x<128;x++){
    SetPoint(&dev,x,0);
    SetPoint(&dev,x,63);
  }
  for(u32 y=0;y<63;y++){
    SetPoint(&dev,0,y);
    SetPoint(&dev,127,y);
  }
  
  while(1)
  {
    static u32 yPos = 1;
    unsigned long xPos = 10;
    yPos++;
    if(yPos == 47){
      yPos = 1;
    }
    /* Cleare previous display data */
    TextOut(&dev,xPos,yPos>1?yPos-1:46,"              ",0xff);
    /* Output the English characters "Hello" */
    xPos = TextOut(&dev,xPos,yPos,"Hello ",0xFF);
    /* Output the Chinese characters "世界" */
    xPos = SpecTextOut(&dev,xPos,yPos,world,2);
    /* Output the '!' sign */
    xPos = TextOut(&dev,xPos,yPos,"!",0xFF);
    
    for(u32 i=2000000;--i;);
  }
}

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
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configure the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
  /* 2 bits for pre-emption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/*******************************************************************************
* Function Name  : SSD1303_IO_Configuration
* Description    : Configure the IO used for SSD1303.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SSD1303_IO_Configuration(void)
{ 
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable SPI1 and GPIO clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  
  SPI_I2S_DeInit(SPI2);
  
  /* Configure SPI2 pins: SCK, MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* Configure SSD1303 pins: RES,A0*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* SPI1 configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  //SPI_Init(SPI1, &SPI_InitStructure);
  SPI_Init(SPI2, &SPI_InitStructure);
  
  SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);

  /* Enable SPI1  */
  SPI_Cmd(SPI2, ENABLE);
}

/*******************************************************************************
* Function Name  : SSD1303_Controller_Init
* Description    : Initialize the oled controller, which is simulated by STM32.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  SSD1303_Controller_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  DMA_DeInit(DMA1_Channel5);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&SPI2->DR);
  DMA_InitStructure.DMA_MemoryBaseAddr = 0;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
  
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
  SysTick_SetReload(72000000/SSD1303_FPS);
  SysTick_ITConfig(ENABLE);
  SysTick_CounterCmd(SysTick_Counter_Enable);
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
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
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
