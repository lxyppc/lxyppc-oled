/*******************************************************************************
* File Name          : DrawText.c
* Author             : lxyppc
* Version            : V1.0
* Date               : 10-01-21
* Description        : Text output implemention file
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "DrawText.h"
#include "font.h"
#include "SSD1303.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
Pos_t  DrawChar(Device* pDev, Pos_t x, Pos_t y, char ch);

/*******************************************************************************
* Function Name  : TextOut
* Description    : Output a text at specify position
* Input          : Device*          device
*                  Pos_t            x location
*                  Pos_t            y location
*                  char*            text
*                  Size_t           text length
* Output         : None
* Return         : End x position for the last character
*******************************************************************************/
Pos_t  TextOut(Device* pDev, Pos_t x, Pos_t y, const char* text, Size_t len)
{
  while(*text && len){
    x += DrawChar(pDev,x,y,*text);
    text++;
    len--;
  }
  return x;
}

/*******************************************************************************
* Function Name  : SpecTextOut
* Description    : Output a specify text at specify position
* Input          : Device*          device
*                  Pos_t            x location
*                  Pos_t            y location
*                  const FontData*  text dotmatrix data
*                  Size_t           text length
* Output         : None
* Return         : End x position for the last character
*******************************************************************************/
Pos_t SpecTextOut(Device* pDev, Pos_t x, Pos_t y, const FontData* textDot, Size_t len)
{
  const FontData* old = pDev->font;
  pDev->font = textDot;
  for(unsigned long i=0;i<len;i++){
    x += DrawChar(pDev,x,y,i);
  }
  pDev->font = old;
  return x;
}

/*******************************************************************************
* Function Name  : DrawChar
* Description    : Output a character at specify position
* Input          : Device*          device
*                  Pos_t            x location
*                  Pos_t            y location
*                  char             character
* Output         : None
* Return         : End x position for the last character
*******************************************************************************/
Pos_t  DrawChar(Device* pDev, Pos_t x, Pos_t y, char ch)
{
  const FontData* ft = &pDev->font[ch];
  pDev->pDevProp->pfnDrawBlok(x,y,ft->width,ft->height,ft->data);
  return ft->width;
}

/*******************************************************************************
* Function Name  : InitialDevice
* Description    : Initialize the device strcture
* Input          : const DeviceProp*  device properties
*                  const FontData*    font
* Output         : Device*          device pointer
* Return         : End x position for the last character
*******************************************************************************/
void  InitialDevice(Device* pDev, const DeviceProp* pDevProp, const FontData* pFont)
{
  pDev->pDevProp = pDevProp;
  pDev->font = pFont;
  pDev->curX = 0;
  pDev->curY = 0;
}

/*******************************************************************************
* Function Name  : SetPoint
* Description    : Draw a point at specify position
* Input          : Device*          device
*                  Pos_t            x location
*                  Pos_t            y location
* Output         : None
* Return         : End x position for the last character
*******************************************************************************/
unsigned long SetPoint(
  Device* pDev,
  Pos_t x,
  Pos_t y)
{
  return pDev->pDevProp->pfnDrawPoint(x,y,1);
}

/*******************************************************************************
* Function Name  : ClearPoint
* Description    : Clear a point at specify position
* Input          : Device*          device
*                  Pos_t            x location
*                  Pos_t            y location
* Output         : None
* Return         : End x position for the last character
*******************************************************************************/
unsigned long ClearPoint(
  Device* pDev,
  Pos_t x,
  Pos_t y)
{
  return pDev->pDevProp->pfnDrawPoint(x,y,0);
}
