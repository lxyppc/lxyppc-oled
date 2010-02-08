/*******************************************************************************
* File Name          : SSD1303.c
* Author             : lxyppc
* Version            : V1.0
* Date               : 10-01-21
* Description        : SSD1303 operations
*                      the SSH1101A is compatible with SSD1303
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "bsp.h"
#include "SSD1303.h"
#include "DrawText.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define   SSD1303_PAGE_NUMBER           8
#define   SSD1303_COLUMN_NUMBER         128
#define   SSD1303_COLUMN_MARGIN_START   2
#define   SSD1303_COLUMN_MARGIN_END     2
#define   SSD1303_X_PIXEL   128
#define   SSD1303_Y_PIXEL   64

/* Private macro -------------------------------------------------------------*/
#define   SSD1303_Buffer    (_SSD1303_Buffer + SSD1303_COLUMN_MARGIN_START)

/* Private variables ---------------------------------------------------------*/
static  u8  _SSD1303_Buffer[SSD1303_COLUMN_NUMBER*SSD1303_PAGE_NUMBER 
+ SSD1303_COLUMN_MARGIN_START + SSD1303_COLUMN_MARGIN_END] = {0};
static  u8  pageIndex = 0;

/* Private function prototypes -----------------------------------------------*/
void  WriteCommand(unsigned char command);
void  WriteData(unsigned char data);
void  OnPageTransferDone(void);

/*******************************************************************************
* Function Name  : WriteCommand
* Description    : Write command to the SSD1303
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WriteCommand(unsigned char command)
{
  SSD_A0_Low();
  SPI_SendByte(command);
  SPI_Wait();
}

/*******************************************************************************
* Function Name  : WriteData
* Description    : Write data to the SSD1303
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WriteData(unsigned char data)
{
  SSD_A0_High();
  SPI_SendByte(data);
  SPI_Wait();
}

/*******************************************************************************
* Function Name  : SSD1303_Init
* Description    : Initialize the SSD1303
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SSD1303_Init(void)
{
  /* Generate a reset */
  SSD_Reset_Low();
  for(u32 i=5000;--i;);//ÑÓÊ±5uSÒÔÉÏ
  SSD_Reset_High();
  
  /*************************************************
  // SSD1303 Initialization Command
  *************************************************/
  // Lower Column Address
  WriteCommand(0x00); /* Set Lower Column Address */
  // High Column Address
  WriteCommand(0x10); /* Set Higher Column Address*/
  // Display Start Line
  WriteCommand(0x40); /* Set Display Start Line */
  // Contrast Control Register
  WriteCommand(0x81); /* Set Contrast Control */
  WriteCommand(0x1f); /* 0 ~ 255 0x1f*/
  // Re-map
  WriteCommand(0xA1); /* [A0]:column address 0 is map 
  to SEG0 , [A1]: columnaddress 131 is map to SEG0*/ 
  // Entire Display ON/OFF
  WriteCommand(0xA4); /* A4=ON */
  // Normal or Inverse Display
  WriteCommand(0XA6); /* Normal Display*/
  // Multiplex Ratio
  WriteCommand(0xA8); /* Set Multiplex Ratio */
  WriteCommand(0x3f); /* Set to 36 Mux*/
  // Set DC-DC
  WriteCommand(0xAD); /* Set DC-DC */
  WriteCommand(0x8B); /* 8B=ON, 8A=Off */
  
  // Display ON/OFF
  WriteCommand(0xAE); /* AF=ON , AE=OFF*/
  // Display Offset
  WriteCommand(0xD3); /* Set Display Offset */
  WriteCommand(0x00); /* No offset */
  // Display Clock Divide
  WriteCommand(0xD5); /* Set Clock Divide */
  WriteCommand(0x20); /* Set to 80Hz */
  // Area Color Mode
  WriteCommand(0xD8); /* Set Area Color On or Off*/
  WriteCommand(0x00); /* Mono Mode */
  // COM Pins Hardware Configuration
  WriteCommand(0xDA); /* Set Pins HardwareConfiguration */
  WriteCommand(0x12);
  // VCOMH
  WriteCommand(0xDB); /* Set VCOMH */
  WriteCommand(0x00);
  // VP
  WriteCommand(0xD9); /* Set VP */
  WriteCommand(0x22); /* P1=2 , P2=2 */
  
  // Set Common output scan direction
  WriteCommand(0xc8);/* Set COM scan direction */
  
  // Set Charge pump
  WriteCommand(0x80); /* Set Charge pump */
  WriteCommand(0x14); /* 0x14=ON, 0x10=Off */
  
  // Turn on the display
  WriteCommand(0xaf);
}

/*******************************************************************************
* Function Name  : OnPageTransferDone
* Description    : Called when each page transfer done
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  OnPageTransferDone(void)
{
  if(pageIndex == SSD1303_PAGE_NUMBER){
    pageIndex = 0;
    return;
  }
  WriteCommand(0xb0 + pageIndex);
  if(pageIndex == 0){
    WriteCommand(0x00);
    WriteCommand(0x10);
  }
  SSD_A0_High();
  DMA1_Channel5->CCR &= ((u32)0xFFFFFFFE);
  DMA1_Channel5->CNDTR = SSD1303_COLUMN_NUMBER+SSD1303_COLUMN_MARGIN_START + SSD1303_COLUMN_MARGIN_END;
  DMA1_Channel5->CMAR = (u32)(SSD1303_Buffer+SSD1303_COLUMN_NUMBER*pageIndex - SSD1303_COLUMN_MARGIN_START);
  //DMA_Cmd(DMA1_Channel5, ENABLE);
  DMA1_Channel5->CCR |= ((u32)0x00000001);
  pageIndex++;
}

/*******************************************************************************
* Function Name  : DMA1_Channel5_IRQHandler
* Description    : This function handles DMA1 Channel 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_IT_TC5)){
    DMA_ClearITPendingBit(DMA1_IT_GL5);
    OnPageTransferDone();
  }
}

/*******************************************************************************
* Function Name  : StartPageTransfer
* Description    : Start a new page transfer
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void StartPageTransfer(void)
{
  pageIndex = 0;
  OnPageTransferDone();
}

/*******************************************************************************
  Display related functions
 ******************************************************************************/
/*******************************************************************************
* Function Name  : SSD1303_DrawBlock
* Description    : Draw a block with specify data to SSD1303
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned long SSD1303_DrawBlock(
  Pos_t x,
  Pos_t y,
  Pos_t cx,
  Pos_t cy,
  const unsigned char* data)
{
  if(x+cx > SSD1303_X_PIXEL)return 0;
  if(y+cy > SSD1303_Y_PIXEL)return 0;
  unsigned char* pStart = SSD1303_Buffer + (y/8)*SSD1303_COLUMN_NUMBER;
  unsigned char offset = y%8;
  unsigned char mask = (1<<offset)-1;
  
  if(!offset){
    for(Pos_t i=0;i<cx;i++){
      unsigned char tmp;
      for(Pos_t j=0;j<cy/8;j++){
        tmp = data[i+j*cx];
        *(pStart + j*SSD1303_COLUMN_NUMBER + i + x) = tmp;
      }
    }
  }else{
    for(Pos_t i=0;i<cx;i++){
      unsigned short tmp;
      for(Pos_t j=0;j<cy/8;j++){
        if(j == 0){
          // First line
          tmp = *(pStart + j*SSD1303_COLUMN_NUMBER + i + x) & mask;
        }
        tmp |= (((unsigned short)data[i+j*cx])<<offset);
        *(pStart + j*SSD1303_COLUMN_NUMBER + i + x) = tmp;
        tmp>>=8;
        if(j == (cy/8)-1){
          // Last line
          *(pStart + (j+1)*SSD1303_COLUMN_NUMBER + i + x) &= ~mask;
          *(pStart + (j+1)*SSD1303_COLUMN_NUMBER + i + x) |= tmp;
        }
      }
    }
  }
  
  return 0;
}

/*******************************************************************************
* Function Name  : SSD1303_DrawPoint
* Description    : Draw a point with specify data to SSD1303
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned long SSD1303_DrawPoint(
  Pos_t x,
  Pos_t y,
  Color_t color)
{
  if(x >= SSD1303_X_PIXEL)return 0;
  if(y >= SSD1303_Y_PIXEL)return 0;
  unsigned char* pStart = SSD1303_Buffer + (y/8)*SSD1303_COLUMN_NUMBER + x;
  unsigned char mask = 1<<(y%8);
  if(color){
    *pStart |= mask;
  }else{
    *pStart &= ~mask;
  }
  return 0;
}

/*******************************************************************************
* Function Name  : SSD1303_ReadPoint
* Description    : Read a point from SSD1303
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned long SSD1303_ReadPoint(
  Pos_t x,
  Pos_t y)
{
  unsigned char* pStart = SSD1303_Buffer + (y/8)*SSD1303_COLUMN_NUMBER + x;
  unsigned char mask = 1<<(y%8);
  return *pStart&mask ? 1 : 0;
}

/*******************************************************************************
* Function Name  : SSD1303_ClearScreen
* Description    : Clear the screen contents
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  SSD1303_FillScreen(Color_t color)
{
  unsigned char mask = color ? 0xFF : 0;
  for(unsigned long i=0;i<SSD1303_COLUMN_NUMBER*SSD1303_PAGE_NUMBER;i++){
    SSD1303_Buffer[i] = mask;
  }
}

const DeviceProp  SSD1303_Prop =
{
  .pfnDrawBlok = SSD1303_DrawBlock,
  .pfnDrawPoint = SSD1303_DrawPoint,
  .xPixel = 128,
  .yPixel = 64,
};

