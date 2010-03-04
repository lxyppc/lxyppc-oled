/*******************************************************************************
* File Name          : Icon.c
* Author             : lxyppc
* Version            : V1.0
* Date               : 10-03-04
* Description        : Icons implementions
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "DrawText.h"
#include "font.h"
#include "ICON.h"
#include "SSD1303.h"
/* Private typedef -----------------------------------------------------------*/
typedef struct _Icon
{
  unsigned char width;
  unsigned char height;
  unsigned char data[];
}ICON_t;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static const unsigned char ICON_Charge_data[] = {
  /* Width */ 21,
  /* Height*/ 16,
  /* Image data */
  255,1,253,253,253,253,237,109,37,129,201,237,237,253,253,253,253,1,255,72,120
  ,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,0,0,
};

static const unsigned char ICON_Power_data[] = {
  /* Width */ 21,
  /* Height*/ 16,
  /* Image data */
  255,255,255,255,207,207,207,135,3,3,3,3,183,183,255,255,255,255,255,72,120,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,0,
};

static const unsigned char ICON_Battary_data[] = {
  /* Width */ 21,
  /* Height*/ 16,
  /* Image data */
  255,1,253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,1,255,72,120,
  3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,0,0,
};

static const unsigned char* IOCN_Table[ICON_ID_MAX] = {
  ICON_Charge_data,   // ICON_ID_CHARGE
  ICON_Power_data,    // ICON_ID_POWER
  ICON_Battary_data,  // ICON_ID_BATTARY
};


unsigned long DrawIcon(Pos_t x, Pos_t y, unsigned long ICON_ID)
{
  if(ICON_ID>ICON_ID_MAX){
    return x;
  }
  const unsigned char* pIcon = IOCN_Table[ICON_ID-1];
  return SSD1303_DrawBlock(x,y,pIcon[0],pIcon[1],pIcon+2);
}
