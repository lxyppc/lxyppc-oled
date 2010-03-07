/*******************************************************************************
* File Name          : menu.c
* Author             : lxyppc
* Version            : V1.0
* Date               : 10-03-05
* Description        : Menu implement
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "menu.h"
#include "SSD1303.h"
#include "Graphics\Graphics.h"
#include "time.h"
#include "icon.h"
#include "bsp.h"
#include "ClockUI.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static  const   MenuItem*    curMenuItem;
static  u8      preQuit = 0;
extern  const LPCSTR* curLang;
extern  const MenuItem  MainMenu[];
extern  const MenuItem  RootMenu;
static  MenuFunc_t menuProc = 0;
extern  Device    appDevice;
/* Private function prototypes -----------------------------------------------*/
void  UpdateMenu(void);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : InitialMenu
* Description    : Initialize the menu
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
MenuFunc_t    InitialMenu()
{
  InitialDevice(&appDevice,&SSD1303_Prop,SongSmall5_DrawChar);
  curMenuItem = MainMenu;
  menuProc = RootMenu.pfnMenu;
  Msg msg = {.param = 0, .message = MSG_INIT};
  menuProc(0,&msg);
  return MR_Finish;
}

unsigned long  MenuProcess(Msg* msg)
{
  if(menuProc){
    if(menuProc((void*)curMenuItem,msg) == MR_Finish){
      menuProc = 0;
      UpdateMenu();
    }
    return 0;
  }
  unsigned char menuUpdate = 0;
  switch(msg->message){
  case MSG_SCROLL:
    if((signed long)msg->param > 0){
      // Scroll down
      if(preQuit){
        preQuit = 0;
        menuUpdate = 1;
      }else{
        unsigned char idx = curMenuItem->index+1;
        if(idx >= curMenuItem->cnt){
          //curMenuItem = curMenuItem - curMenuItem->index;
        }else{
          curMenuItem++;
          menuUpdate = 1;
        }
      }
    }else{
      // Scroll up
      if(curMenuItem->index){
        curMenuItem--;
        menuUpdate = 1;
      }else{
        menuUpdate = !preQuit;
        preQuit = 1;
      }
    }
    break;
  case MSG_KEY_UP:
    if(preQuit){
      preQuit = 0;
      curMenuItem = curMenuItem->parent;
      menuUpdate = 1;
      if(!curMenuItem){
        menuProc = RootMenu.pfnMenu;
      }
    }else{
      if(curMenuItem->type & MT_SUB){
        curMenuItem = curMenuItem->child;
        menuUpdate = 1;
      }else{
        menuProc = curMenuItem->pfnMenu;
      }
    }
    break;
  default:
    return 0;
  }
  if(menuProc){
    Msg ini = { .param = 0, .message = MSG_INIT};
    menuProc((void*)curMenuItem,&ini);
  }else{
    if(menuUpdate){
      UpdateMenu();
    }
  }
  return 0;
}

#define   MARGIN          1
#define   LEFT_MARGIN     12
#define   MENU_HEIGHT     12
void  UpdateMenu(void)
{
  const char* text;
  if(!curMenuItem)return;
  SetColor(BLACK);
  ClearDevice();
  if(curMenuItem->parent){
    text = curLang[curMenuItem->parent->res];
  }else{
    text = curLang[RootMenu.res];
  }
  Pos_t x;
  if(preQuit){
    x = TextOut_HighLight(&appDevice, 0, 0, text ,0xFF);
  }else{
    x = TextOut(&appDevice, 0, 0, text ,0xFF);
  }
  TextOut(&appDevice, x, 0, "<<" ,0xFF);
  
  Pos_t y = MENU_HEIGHT + MARGIN;
  const MenuItem* items = curMenuItem;
  if(curMenuItem->index){
    items--;
  }
  for(u32 i=0; i<4; i++){
    text = curLang[items->res];
    if(items == curMenuItem && !preQuit){
      TextOut_HighLight(&appDevice, LEFT_MARGIN, y, text ,0xFF);
    }else{
      TextOut(&appDevice, LEFT_MARGIN, y, text ,0xFF);
    }
    if(items->index + 1 == items->cnt){
      break;
    }
    items++;
    y += MENU_HEIGHT + MARGIN;
  }
}

MenuResult     PopupMenu(const MenuItem* menu)
{
  curMenuItem = menu;
  return MR_Finish;
}

/**
    Message realted definitions
 */
#define     MSG_STACK_SIZE    16
#define     MSG_STACK_MASK    15
static  unsigned char wrMsgIndex = 0;
static  unsigned char rdMsgIndex = 0;
static  Msg     msgStack[MSG_STACK_SIZE];
/*******************************************************************************
* Function Name  : GetMessage
* Description    : Get message
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned long   GetMessage(Msg* msg)
{
  if(wrMsgIndex != rdMsgIndex){
    Msg* p = &msgStack[rdMsgIndex & MSG_STACK_MASK];
    msg->message = p->message;
    msg->param = p->param;
    rdMsgIndex++;
    return 1;
  }
  return 0;
}

unsigned long   PostMessage(Msg* msg)
{
  Msg* p = &msgStack[wrMsgIndex & MSG_STACK_MASK];
  p->message = msg->message;
  p->param = msg->param;
  wrMsgIndex++;
  return 0;
}
