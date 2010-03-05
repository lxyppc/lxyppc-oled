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
typedef   const char*    LPCSTR;

/* Private define ------------------------------------------------------------*/
#define   RES_ROOT              0
#define   RES_MEASURE           1
#define   RES_TEST              2
#define   RES_SETTING           3
#define   RES_ABOUT             4

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static  const   MenuItem*    curMenuItem;
static  u8      preQuit = 0;
static  Device  menuDev;
static  LPCSTR MenuText_En[] = 
{
  "Quit",       /* RES 0 */
  "Measure",    /* RES_MEASURE */
  "Test",       /* RES 2 */
  "Setting",    /* RES 3 */
  "About",      /* RES 4 */
};
static  LPCSTR* curLang = MenuText_En;

extern  const MenuItem    measureMenu[];
extern  const MenuItem    mainMenu[];
unsigned long OnMenuAbout(void* p, Msg* msg);

const MenuItem    measureMenu[] = {
  /*Parent          child/pfn           index      cnt        resource        type*/
  {   mainMenu,     measureMenu,         0,        4,        RES_MEASURE,    MT_SUB},
  {   mainMenu,     (void*)OnMenuAbout,  1,        4,        RES_TEST,       MT_NULL},
  {   mainMenu,     (void*)OnMenuAbout,  2,        4,        RES_SETTING,    MT_NULL},
  {   mainMenu,     (void*)OnMenuAbout,  3,        4,        RES_ABOUT,      MT_NULL},
};


unsigned long OnMenuAbout(void* p, Msg* msg)
{
  if(msg->message == MSG_INIT){
    SetColor(BLACK);
    ClearDevice();
    TextOut(&menuDev, 0, 0,curLang[RES_ABOUT], 0xFF);
    return 0;
  }else if(msg->message == MSG_KEY_UP){
    return 1;
  }
  return 0;
}

unsigned long OnMenuTest(void* p, Msg* msg)
{
  if(msg->message == MSG_INIT){
    SetColor(BLACK);
    ClearDevice();
    TextOut(&menuDev, 0, 0,curLang[RES_TEST], 0xFF);
    return 0;
  }else if(msg->message == MSG_KEY_UP){
    return 1;
  }else if(msg->message == MSG_SCROLL){
    s32 res = (s32)msg->param;
    char buf[3] = "+";
    if(res<0){
      res = -res;
      buf[0] = '-';
    }
    static Pos_t x = 0;
    static Pos_t y = 13;
    buf[1] = res%10+'0';
    buf[2] = ',';
    x = TextOut(&menuDev, x, y, buf, 3);
    if(x > 110){
      x = 0;
      y+=13;
    }
    if( y > 64-13){
      y = 13;
    }
    
  }
  return 0;
}



const MenuItem    mainMenu[] = {
  /*Parent    child/pfn           index      cnt        resource        type*/
  {   0,     measureMenu,         0,        4,        RES_MEASURE,    MT_SUB},
  {   0,     (void*)OnMenuTest,   1,        4,        RES_TEST,       MT_NULL},
  {   0,     (void*)OnMenuAbout,  2,        4,        RES_SETTING,    MT_NULL},
  {   0,     (void*)OnMenuAbout,  3,        4,        RES_ABOUT,      MT_NULL},
};

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
  InitialDevice(&menuDev,&SSD1303_Prop,fontBuffer_fixedsys);
  curMenuItem = mainMenu;
  UpdateMenu();
  return 0;
}

unsigned long RootApp(void* param, Msg* msg)
{
  if(msg->message == MSG_INIT){
    SetColor(BLACK);
    ClearDevice();
    TextOut(&menuDev, 0, 0,curLang[RES_MEASURE], 0xFF);
  }else if(msg->message == MSG_KEY_UP){
    curMenuItem = mainMenu;
    return 1;
  }else if(msg->message == MSG_SECOND){
    static u32 i = 0;
    char buf[] = {i%10+'0', 0};
    TextOut(&menuDev, 24, 24,buf, 0xFF);
    i++;
  }
  return 0;
}

unsigned long  MenuProcess(Msg* msg)
{
  static MenuFunc_t proc = 0;
  if(proc){
    if(proc((void*)curMenuItem,msg)){
      proc = 0;
      UpdateMenu();
    }
    return 0;
  }
  switch(msg->message){
  case MSG_SCROLL:
    if((signed long)msg->param > 0){
      // Scroll down
      if(preQuit){
        preQuit = 0;
      }else{
        unsigned char idx = curMenuItem->index+1;
        if(idx >= curMenuItem->cnt){
          curMenuItem = curMenuItem - curMenuItem->index;
        }else{
          curMenuItem++;
        }
      }
    }else{
      // Scroll up
      if(curMenuItem->index){
        curMenuItem--;
      }else{
        preQuit = 1;
      }
    }
    break;
  case MSG_KEY_UP:
    if(preQuit){
      preQuit = 0;
      curMenuItem = curMenuItem->parent;
      if(!curMenuItem){
        proc = RootApp;
      }
    }else{
      if(curMenuItem->type & MT_SUB){
        curMenuItem = curMenuItem->child;
      }else{
        proc = curMenuItem->pfnMenu;
      }
    }
    break;
  default:
    break;
  }
  if(proc){
    Msg ini = { .param = 0, .message = MSG_INIT};
    proc((void*)curMenuItem,&ini);
  }else{
    UpdateMenu();
  }
  return 0;
}

#define   MARGIN          1
#define   LEFT_MARGIN     12
#define   MENU_HEIGHT     12
void  UpdateMenu(void)
{
  const char* text;
  SetColor(BLACK);
  ClearDevice();
  if(curMenuItem->parent){
    text = curLang[curMenuItem->parent->res];
  }else{
    text = curLang[RES_ROOT];
  }
  Pos_t x;
  if(preQuit){
    x = TextOut_HighLight(&menuDev, 0, 0, text ,0xFF);
  }else{
    x = TextOut(&menuDev, 0, 0, text ,0xFF);
  }
  TextOut(&menuDev, x, 0, "<<" ,0xFF);
  
  Pos_t y = MENU_HEIGHT + MARGIN;
  const MenuItem* items = curMenuItem;
  if(curMenuItem->index){
    items--;
  }
  for(u32 i=0; i<4; i++){
    text = curLang[items->res];
    if(items == curMenuItem && !preQuit){
      TextOut_HighLight(&menuDev, LEFT_MARGIN, y, text ,0xFF);
    }else{
      TextOut(&menuDev, LEFT_MARGIN, y, text ,0xFF);
    }
    if(items->index + 1 == items->cnt){
      break;
    }
    items++;
    y += MENU_HEIGHT + MARGIN;
  }
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
