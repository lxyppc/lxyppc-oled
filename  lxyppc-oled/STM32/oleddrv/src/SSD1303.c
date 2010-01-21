#include "stm32f10x_lib.h"
#include "bsp.h"
#include "SSD1303.h"

#define   SlightDelay()   //for(u32 j=5;--j;);

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
  SlightDelay();
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
  SlightDelay();
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
  for(u32 i=5000;--i;);//—” ±5uS“‘…œ
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
  WriteCommand(0xc0);/* Set COM scan direction */
}
