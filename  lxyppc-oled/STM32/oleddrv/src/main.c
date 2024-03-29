/*******************************************************************************
* File Name          : main.c
* Author             : lxyppc
* Version            : V1.0
* Date               : 10-01-21
* Description        : Main program body
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "bsp.h"
#include "SSD1303.h"
#include "Graphics\Graphics.h"  // Graphic primitives layer
#include "ClockUI.h"
#include "ClockSet.h"
#include "Encoder.h"
#include "..\..\oledLoader\Inc\Export.h"
#include "time.h"
#include "icon.h"
#include "menu.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define   RTCClockSource_LSI
#define   RTCClockSource_LSE

/* Private macro -------------------------------------------------------------*/
#define WAIT_UNTIL_FINISH(x) (x)

/* Private variables ---------------------------------------------------------*/
static  u32  minute = 0;
static  s16 encCount = 0;
static vu32 TimeDisplay = 0;
static  const char HexTable[] = "0123456789ABCDEF";
ADResult_t    ADCResult;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void SSD1303_IO_Configuration(void);
void SSD1303_Controller_Init(void);
void CCW_Rotate(unsigned char *des, const unsigned char *src);
void RTC_Configuration(void);
void Initial_Tim2(void);
void InitialIO(void);
void InitialADC(void);
void InitialSystem();
#ifdef    DEBUG_UI
u32  InitialUIDebugger(u16 width, u16 height, u16 depth, u8* buffer);
#endif
/* Private functions ---------------------------------------------------------*/
void  GraphTest(void)
{
  SHORT counter;
  Device dev;
  InitialDevice(&dev,&SSD1303_Prop,SongSmall5_DrawChar);
  
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
    
      /* Draw a box */
    for(u32 x=0;x<128;x++){
      SetPoint(&dev,x,0);
      SetPoint(&dev,x,63);
    }
    for(u32 y=0;y<63;y++){
      SetPoint(&dev,0,y);
      SetPoint(&dev,127,y);
    }
    static u32 yPos = 1;
    yPos = 0;
    while(1){
      unsigned long xPos = 10;
      yPos++;
      if(yPos == 47){
        break;
      }
      /* Cleare previous display data */
      TextOut(&dev,xPos,yPos>1?yPos-1:46,"              ",0xff);
      /* Output the English characters "Hello" */
      xPos = TextOut(&dev,xPos,yPos,"Hello ",0xFF);
      /* Output the Chinese characters "����" */
      xPos = TextOut(&dev,xPos,yPos,"World",2);
      /* Output the '!' sign */
      //xPos = TextOut(&dev,xPos,yPos,"!",0xFF);
      
      TextOut(&dev,xPos + 8,8, (yPos & 2) ? (yPos&1 ? "\\" : "-" ) : (yPos&1 ? "/" : "|" ),0xFF);
      
      for(u32 i=2000000;--i;);
    }
    SetColor(BLACK);
    ClearDevice();
    break;
  }
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
#ifdef DEBUG
  debug();
#endif
  {
    /*
      Todo: the app will crash if remove the follow codes
    */
    time_t now = 0;
    struct tm* curTm = localtime(&now);
  }
  
  InitialSystem();
  //GraphTest();
  Device dev;
  InitialDevice(&dev,&SSD1303_Prop,SongSmall5_DrawChar);
  
  //SpecTextOut(&dev,1,10,song_6_fontBuffer,7);
  //while(1);
  InitialMenu();
  while(1){
    static  Msg msg;
    if(GetMessage(&msg)){
      MenuProcess(&msg);
    }
  }
  
  Clock_DrawFace(GetMaxY()>>1,GetMaxY()>>1,GetMaxY()>>1);
  //Line(counter,0,GetMaxX()-1-counter,GetMaxY()-1);
  while(1){
    static  Msg msg;
    if(!GetMessage(&msg))continue;
//    if(minute != lastM){
//      lastM = minute;
//      char buf[] = {minute/10+'0',minute%10+'0',0};
//      TextOut(&dev,80,20,buf,0xff);
//      if(encCount<0){
//        encCount = -encCount;
//        char buf[] = {'-',encCount/10+'0',encCount%10+'0',0};
//        TextOut(&dev,100,20,buf,0xff);
//      }else{
//        char buf[] = {'+',encCount/10+'0',encCount%10+'0',0};
//        TextOut(&dev,100,20,buf,0xff);
//      }
//    }
    if(msg.message == MSG_KEY_UP){
      if(SSD1303_IsOn()){
        SSD1303_TurnOff();
      }else{
        SSD1303_TurnOn();
      }
      char buf[] = {minute/10+'0',minute%10+'0',0};
      TextOut(&dev,80,40,buf,0xff);
    }
    char buf[64];
    if(ReadUsbData(buf,64) == LDR_SUCCESS){
      if(buf[1] == 0xAA){
        RTC_SetCounter(*(u32*)(buf+4));
        RTC_WaitForLastTask();
      }
    }
    if(msg.message == MSG_SECOND){
    //if(TimeDisplay){
      CheckConnection();
      vu16 ccr1 = TIM3->CCR1;
      Pos_t x = 64;
      Pos_t y = 16;
      TimeDisplay = 0;
      u32 TimeVar = RTC_GetCounter();
      u32 THH = 0, TMM = 0, TSS = 0;
      /* Compute  hours */
      THH = TimeVar / 3600;
      /* Compute minutes */
      TMM = (TimeVar % 3600) / 60;
      /* Compute seconds */
      TSS = (TimeVar % 3600) % 60;
      char tBuf[64] = {1};
      //x = TextOut(&dev,x,y,tBuf+1,0xff);
      Clock_UpdateTime(THH,TMM,TSS);
      time_t now = (time_t)TimeVar;
      char* p = ctime(&now);
      for(u32 i=0;i<26;){
        i++;
        tBuf[i] = *p++;
      }
      TextOut_HighLight(&dev,x,y,tBuf+12,8);
      TextOut(&dev,x,y+16,tBuf+5,6);
      TextOut(&dev,x,y+32,tBuf+21,4);
      TextOut(&dev,x+32,y+32,tBuf+20,1);
      TextOut_HighLight(&dev,x+40,y+32,tBuf+1,3);
      if(0){
        u32 res = ADCResult.ADBat;
        res = res * 328 * 136 / (4096*100);
//        char ADBuf[] = {'0', 'x', HexTable[(res>>12)&0xF],
//        HexTable[(res>>8)&0xF],HexTable[(res>>4)&0xF],HexTable[res&0xF],0};
        
        char ADBuf[] = {'B', 'a', 't', HexTable[(res/100)%10], '.', 
        HexTable[(res/10)%10], HexTable[res%10],'V', 0};
        y+=16;
        TextOut(&dev,x,y,ADBuf,8);
      }
      if(0){
        u16 res = ADCResult.ADX;
        char ADBuf[] = {'0', 'x', HexTable[(res>>12)&0xF],
        HexTable[(res>>8)&0xF],HexTable[(res>>4)&0xF],HexTable[res&0xF],
        ' ', IsPGOOD() ? 'P' : 'X', 0};
        y+=16;
        TextOut(&dev,x,y,ADBuf,8);
      }
      if(0){
        u16 res = ADCResult.ADY;
        char ADBuf[] = {'0', 'x', HexTable[(res>>12)&0xF],
        HexTable[(res>>8)&0xF],HexTable[(res>>4)&0xF],HexTable[res&0xF],
        ' ', IsCHG() ? 'C' : 'X', 0};
        y+=16;
        TextOut(&dev,x,y,ADBuf,8);
      }
      
      static unsigned char icoY = 0;
      //icoY++;
      //if(icoY==64)icoY=0;
      if(IsCHG()){
        DrawIcon(104,icoY,ICON_ID_CHARGE);
      }else if(IsPGOOD()){
        DrawIcon(104,icoY,ICON_ID_POWER);
      }else{
        DrawIcon(104,0,ICON_ID_BATTARY);
        // Battary check
        // Full charge is 4.2V, lower power is 3.3V
        // Vref = 3.28V
        // 10K/13.6K
        // 4.2*10/13.6/3.28*4096 = 3875
        // 3.3*10/13.6/3.28*4096 = 3030
        u32 res = ADCResult.ADBat;
        if(res > 3835){
          res = 800;
        }else if(res < 3030){
          res = 0;
        }else{
          res -= 3030;
        }
        res = 16*res/800;
        res += 106;
        SetColor(BLACK);
        SetLineThickness(NORMAL_LINE);
        Bar(res,2,121,7);
      }
      
      WaitAndSendUsbData(tBuf,64,1);
      //ToggleLED();
    }
  }
}

/*******************************************************************************
* Function Name  : InitialSystem
* Description    : Initialize the system
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitialSystem()
{
  GPIO_AFIODeInit();
  GPIO_DeInit(GPIOB);
  /* The bootloader has already initialize the GPIOA*/
  //GPIO_DeInit(GPIOA);
  GPIO_DeInit(GPIOD);
  
  /* NVIC configuration */
  NVIC_Configuration();
  
  /* Initialize GPIO */
  InitialIO();
  
  /* Initialize ADC */
  InitialADC();
  
  /* Initializer system colck */
#ifdef  DEBUG_UI
  InitialUIDebugger(128, 64, 1, SSD1303_GetBuffer());
#else
  RCC_DeInit();
  CheckConnection();
  
  /* Initialize the SSD1303 related IO */
  SSD1303_IO_Configuration();
  
  /* Initialize the SSD1303 */
  SSD1303_Init();
#endif
  
  /* Initialize the SSD1303 controller,
  which is simulated by STM32 DMA and systick*/
  SSD1303_Controller_Init();
  
  /* Initialize the RTC peripheral */
  RTC_Configuration();
  
  /* Initial graph lib*/
  InitGraph();
  
  /* Initial encoder interface*/
  Enc_Init();
}

/*******************************************************************************
* Function Name  : InitialADC
* Description    : Initial the ADC for voltage measure
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitialADC(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
                       | RCC_APB2Periph_GPIOB
                       | RCC_APB2Periph_ADC1
                       | RCC_APB2Periph_ADC2,   ENABLE);
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  /* Configure PC.0(ADC Channel1, Channel2, )
   as analog input -----------------------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  RCC_ADCCLKConfig(RCC_PCLK2_Div6); /// 12MHz for ADC clock
  
  /* Here we config the ADC1 and ADC2  in regular simultaneous mode
     They are trigerred by TIM3 TRGO signal
    The result will stored in ADC1's DR,
  */
  ADC_DeInit(ADC1);
  ADC_DeInit(ADC2);
  
  ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = 
    ADC_ExternalTrigConv_None;
    //ADC_ExternalTrigConv_T3_TRGO;
    //ADC_ExternalTrigConv_T2_CC2;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 3;
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_Init(ADC2, &ADC_InitStructure);
  
  ADC_RegularChannelConfig(ADC1, AD_CH_REF, 1, ADC_SampleTime_13Cycles5);
  ADC_RegularChannelConfig(ADC2, AD_CH_BAT, 1, ADC_SampleTime_13Cycles5);
  ADC_RegularChannelConfig(ADC1, AD_CH_X,   2, ADC_SampleTime_13Cycles5);
  ADC_RegularChannelConfig(ADC2, AD_CH_Y,   2, ADC_SampleTime_13Cycles5);
  ADC_RegularChannelConfig(ADC1, AD_CH_Z,   3, ADC_SampleTime_13Cycles5);
  ADC_RegularChannelConfig(ADC2, AD_CH_CHG, 3, ADC_SampleTime_13Cycles5);
  
  /* Initialize the ADC DMA channel */
  ADC_DMACmd(ADC1,ENABLE);
  DMA_DeInit(DMA_ADC);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&ADC1->DR);
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADCResult;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 3;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//DMA_Priority_Low DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA_ADC, &DMA_InitStructure);
  
  NVIC_InitTypeDef  NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  DMA_ITConfig(DMA_ADC, DMA_IT_TC, ENABLE);
  
  DMA_Cmd(DMA_ADC, ENABLE);
  
  ADC_TempSensorVrefintCmd(ENABLE);
    
  //ADC_ExternalTrigConvCmd(ADC1, ENABLE);
  ADC_ExternalTrigConvCmd(ADC2, ENABLE);
  ADC_Cmd(ADC1, ENABLE);
  ADC_Cmd(ADC2, ENABLE);
  
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
  
  
  ADC_ResetCalibration(ADC2);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC2));
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC2);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC2));
}

/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_IT_TC1)){
    DMA_ClearITPendingBit(DMA1_IT_GL1);
    Msg msg;
    if(Is_MMA_WAKEUP()){
      MakeMsgGrav_XY(&msg,ADCResult.ADX,ADCResult.ADY);
      PostMessage(&msg);
      MakeMsgGrav_Z(&msg,ADCResult.ADZ);
      PostMessage(&msg);
    }
  }
}

/*******************************************************************************
* Function Name  : InitialIO
* Description    : Initial the GPIOs
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitialIO(void)
{
#ifdef  DEBUG_BOARD
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
  
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  
  // PA8 is the led 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  // PB4 for PGOOD signal, PB5 for CHG signal
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  // PB8 for GSel 2, PB9 for GSel2, PB11 for MMA Sleep
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GSel1_Low();
  GSel2_Low();
  LED_OFF();
  MMA_WAKEUP();
  
#else
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
  
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  
  // PA8 is the led 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  // PB4 for PGOOD signal, PB5 for CHG signal
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  // PB8 for GSel 2, PB9 for GSel2, PB11 for MMA Sleep
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GSel1_Low();
  GSel2_Low();
  LED_OFF();
  MMA_SLEEP();
#endif
}

/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define   SCROLL_THRESHOLD      4
void SysTickHandler(void)
{
  Msg msg;
  static s16 lastEncCnt = 0;
  encCount = Enc_GetCount();
  if((lastEncCnt ^ encCount) & 0x8000 ){
    lastEncCnt = encCount;
  }
  lastEncCnt += encCount;
  if(lastEncCnt>SCROLL_THRESHOLD){
    msg.param = (void*)((s32)encCount);
    msg.message = MSG_SCROLL;
    PostMessage(&msg);
    lastEncCnt -= SCROLL_THRESHOLD;
  }else if(lastEncCnt<-SCROLL_THRESHOLD){
    msg.param = (void*)((s32)encCount);
    msg.message = MSG_SCROLL;
    PostMessage(&msg);
    lastEncCnt += SCROLL_THRESHOLD;
  }
  static u32 keyDown = 0;
  if(Is_Enc_Key_Down()){
    if(keyDown == 1){
      msg.param = 0;
      msg.message = MSG_KEY_DOWN;
      PostMessage(&msg);
    }
    keyDown++;
  }else{
    if(keyDown > 2){
      msg.param = (void*)keyDown;
      msg.message = MSG_KEY_UP;
      PostMessage(&msg);
    }
    keyDown = 0;
  }
  
//  if(encCount>0){
//    minute++;
//    if(minute>=60)minute = 0;
//  }else if(encCount<0){
//    if(minute){
//      minute--;
//    }else{
//      minute = 59;
//    }
//  }
#ifndef  DEBUG_UI
  StartPageTransfer();
#endif
  
  if(Is_MMA_WAKEUP()){
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  }
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configure the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  RTC_IRQHandler(void);
void  DMA1_Channel5_IRQHandler(void);
void NVIC_Configuration(void)
{
#if WITH_BOOTLOADER 
  RegisterIrq(-1,SysTickHandler);
  RegisterIrq(RTC_IRQChannel,RTC_IRQHandler);
  RegisterIrq(DMA1_Channel5_IRQChannel,DMA1_Channel5_IRQHandler);
  RegisterIrq(DMA1_Channel1_IRQChannel,DMA1_Channel1_IRQHandler);
#else
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
  /* 2 bits for pre-emption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
#endif
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
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
  
  NVIC_SystemHandlerPriorityConfig(SystemHandler_SysTick,2,0);
  
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
  SysTick_SetReload(72000000/SSD1303_FPS);
  SysTick_ITConfig(ENABLE);
  SysTick_CounterCmd(SysTick_Counter_Enable);
}

/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : Initialize the RTC peripheral.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  
  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);
  
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the RTC Second */  
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask(); 
  
  if(BKP_ReadBackupRegister(BKP_DR1) == 0xA5A5){
    
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
  
    return;
  }
  
  /* Reset Backup Domain */
  BKP_DeInit();
  
#ifdef RTCClockSource_LSI
  /* Enable LSI */ 
  RCC_LSICmd(ENABLE);
  /* Wait till LSI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }

  /* Select LSI as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);  
#elif defined	RTCClockSource_LSE
  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  
#endif
  
  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
 
  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();
  
  /* Set RTC prescaler: set RTC period to 1sec */
#ifdef RTCClockSource_LSI
  RTC_SetPrescaler(31999); /* RTC period = RTCCLK/RTC_PR = (32.000 KHz)/(31999+1) */
#elif defined	RTCClockSource_LSE
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
#endif
  
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  
  RTC_SetCounter(0x4B880000);
  
  RTC_WaitForLastTask();
  
  BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
  {
    /* Clear the RTC Second interrupt */
    RTC_ClearITPendingBit(RTC_IT_SEC);

    /* Toggle GPIO_LED pin 6 each 1s */
    
    Msg msg;
    msg.param = (void*)RTC_GetCounter();
    msg.message = MSG_SECOND;
    PostMessage(&msg);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    if(!Is_MMA_WAKEUP()){
      // When MMA is power off, use the seoond interrupt
      ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    }
  }
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
