/*******************************************************************************
* File Name          : UserMenu.c
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
#include "string.h"
#include "stdlib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Menu resource ID*/
#define   RES_DEFAULT               0
#define   RES_ROOT                  1
#define   RES_STATUS                2
#define   RES_TEST                  3
#define   RES_SETTING               4
#define   RES_ABOUT                 5
#define   RES_SET_CONTRAST          6
#define   RES_SET_TIME              7
#define   RES_STATUS_BAT            8
#define   RES_STATUS_GRAVITY        9

/* Private macro -------------------------------------------------------------*/
//#define     OnMeasureBat      DefaultMenuFunc
//#define     OnMeasureGravity  DefaultMenuFunc
//#define     OnSetContrast     DefaultMenuFunc
//#define     OnSetTime         DefaultMenuFunc

/* Private variables ---------------------------------------------------------*/
extern  const MenuItem    MainMenu[];
extern  const MenuItem    SettingMenu[];
extern  const MenuItem    StatusMenu[];
static  Device    appDevice;
extern  struct{
  u16   ADBat;
  u16   ADX;
  u16   ADY;
  u16   ADZ;
}ADCResult;

LPCSTR MenuText_En[] = 
{
  [RES_DEFAULT]     = "Press to quit...",
  [RES_ROOT]        = "Quit",
  [RES_STATUS]     = "Status",
  [RES_TEST]        = "Test",
  [RES_SETTING]     = "Setting",
  [RES_ABOUT]       = "About",
  [RES_SET_CONTRAST] = "Contrast",
  [RES_SET_TIME]    =  "Time",
  [RES_STATUS_BAT] =  "Battary",
  [RES_STATUS_GRAVITY] = "Gravity",
};

#define   UT_CHANGE_CONTRAST      0
#define   UT_SET_TIME             1
#define   UT_VIEW_BATTARY         2
#define   UT_BAT_CHARGE           3
#define   UT_BAT_FULL             4
#define   UT_BAT_NORMAL           5
#define   UT_BAT_CAPACITY         6
#define   UT_BAT_VOLTAGE          7
LPCSTR  UserText_En[] = 
{
  [UT_CHANGE_CONTRAST]      =     "Change contrast:",
  [UT_SET_TIME]             =     "Set time:",
  [UT_VIEW_BATTARY]         =     "Battary status:",
  [UT_BAT_CHARGE]           =     "Charging ...",
  [UT_BAT_FULL]             =     "Charge finish",
  [UT_BAT_NORMAL]           =     "Disconnected",
  [UT_BAT_CAPACITY]         =     "Capacity:",
  [UT_BAT_VOLTAGE]          =     "Voltage: ",
};

LPCSTR MonthText_En[12] =
{
  "Jan","Feb","Mar","Apr","May","Jun",
  "Jul","Aug","Sep","Oct","Nov","Dec",
};

LPCSTR WeekText_En[9] =
{
  "Sun","Mon","Tue","Wed","Thu","Fri","Sat","am","pm"
};

LPCSTR* MonthText = MonthText_En;
LPCSTR* WeekText = WeekText_En;
LPCSTR* UserText = UserText_En;

/* Private function prototypes -----------------------------------------------*/
MenuResult OnMenuAbout(void* p, Msg* msg);
MenuResult OnMenuTest(void* p, Msg* msg);
MenuResult DefaultMenuFunc(void* p, Msg* msg);
MenuResult RootApp(void* param, Msg* msg);
MenuResult OnSetContrast(void* param, Msg* msg);
MenuResult OnSetTime(void* param, Msg* msg);
MenuResult OnMeasureGravity(void* param, Msg* msg);
MenuResult OnMeasureBat(void* param, Msg* msg);

/* Private functions ---------------------------------------------------------*/

/* Menu definitions */

/* Root main */
const MenuItem    RootMenu = {
        /*Parent    child/pfn           index      cnt        resource        type*/
            0,      (void*)RootApp,         0,        1,        RES_ROOT,    MT_NULL
};
/* Main menu */
const MenuItem    MainMenu[] = {
      /*Parent    child/pfn           index      cnt        resource        type*/
  {         0,     StatusMenu,         0,        4,         RES_STATUS,    MT_SUB},
  {         0,     (void*)OnMenuTest,   1,        4,        RES_TEST,       MT_NULL},
  {         0,     SettingMenu,         2,        4,        RES_SETTING,    MT_SUB},
  {         0,     (void*)OnMenuAbout,  3,        4,        RES_ABOUT,      MT_NULL},
};

/* Setting menu */
const MenuItem    SettingMenu[] = {
      /*Parent    child/pfn                 index      cnt        resource        type*/
  {   MainMenu+2,   (void*)OnSetContrast,     0,        2,        RES_SET_CONTRAST,    MT_NULL},
  {   MainMenu+2,   (void*)OnSetTime,         1,        2,        RES_SET_TIME,        MT_NULL},
};

/**/
const MenuItem    StatusMenu[] = {
  /*Parent          child/pfn               index      cnt        resource        type*/
  {   MainMenu+0,   (void*)OnMeasureBat,      0,        2,        RES_STATUS_BAT,     MT_NULL},
  {   MainMenu+0,   (void*)OnMeasureGravity,  1,        2,        RES_STATUS_GRAVITY, MT_NULL},
};

MenuResult OnMenuAbout(void* p, Msg* msg)
{
  if(msg->message == MSG_INIT){
    SetColor(BLACK);
    ClearDevice();
    InitialDevice(&appDevice,&SSD1303_Prop,fontBuffer_fixedsys);
    TextOut(&appDevice, 0, 12,"Oled Project 1.0", 0xFF);
    TextOut(&appDevice, 0, 25,"lxyppc", 0xFF);
    TextOut(&appDevice, 0, 38,"all right reserved", 0xFF);
    return MR_Continue;
  }else if(msg->message == MSG_KEY_UP){
    return MR_Finish;
  }
  return MR_Continue;
}


MenuResult OnMenuTest(void* p, Msg* msg)
{
  if(msg->message == MSG_INIT){
    SetColor(BLACK);
    ClearDevice();
    InitialDevice(&appDevice,&SSD1303_Prop,fontBuffer_fixedsys);
    TextOut(&appDevice, 0, 0, "Scroll Test", 0xFF);
    return MR_Continue;
  }else if(msg->message == MSG_KEY_UP){
    return MR_Finish;
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
    x = TextOut(&appDevice, x, y, buf, 3);
    if(x > 110){
      x = 0;
      y+=13;
    }
    if( y > 64-12){
      y = 13;
    }
    
  }
  return MR_Continue;
}

MenuResult DefaultMenuFunc(void* p, Msg* msg)
{
  if(msg->message == MSG_INIT){
    SetColor(BLACK);
    ClearDevice();
    InitialDevice(&appDevice,&SSD1303_Prop,fontBuffer_fixedsys);
    TextOut(&appDevice, 0, 25,"press the middle", 0xFF);
    TextOut(&appDevice, 0, 38,"button to quit ...", 0xFF);
    return MR_Continue;
  }else if(msg->message == MSG_KEY_UP){
    return MR_Finish;
  }
  return MR_Continue;
}

unsigned long  GetBatCap()
{
  u32 res = ADCResult.ADBat;
  if(res > 3835){
    res = 800;
  }else if(res < 3030){
    res = 0;
  }else{
    res -= 3030;
  }
  return 100*res/800;
}

void  UpdateBatIcon(Pos_t x, Pos_t y,u32 cap)
{
  if(IsCHG()){
    DrawIcon(x,y,ICON_ID_CHARGE);
  }else if(IsPGOOD()){
    DrawIcon(x,y,ICON_ID_POWER);
  }else{
    DrawIcon(x,y,ICON_ID_BATTARY);
    SetColor(BLACK);
    SetLineThickness(NORMAL_LINE);
    u32 res = cap*16/100;
    res += 106;
    Bar(res,2,121,7);
  }
}

MenuResult RootApp(void* param, Msg* msg)
{
  static  u8  deviceClose = 0;
  switch(msg->message){
  case MSG_INIT:
    SetColor(BLACK);
    ClearDevice();
    InitialDevice(&appDevice,&SSD1303_Prop,fontBuffer_fixedsys);
    Clock_DrawFace(GetMaxY()>>1,GetMaxY()>>1,GetMaxY()>>1);
    deviceClose = 0;
  case MSG_SECOND:
    {
    Pos_t x = 70;
    Pos_t y = 35;
    time_t now = (time_t)RTC_GetCounter();
    struct tm* curTm = localtime(&now);
    Clock_UpdateTime(curTm->tm_hour,curTm->tm_min,curTm->tm_sec);
    u32 year = curTm->tm_year + 1900;
    char tBuf[8] = {year/1000+'0',year/100%10+'0',year/10%10+'0',year%10+'0'};
    x = TextOut(&appDevice,x,y,tBuf,4);
    x = TextOut(&appDevice,x,y," ",1);
    x = TextOut(&appDevice,x,y,WeekText[curTm->tm_wday],3);
    x = 70;
    y += 13;
    x = TextOut(&appDevice,x,y,MonthText[curTm->tm_mon],0xFF);
    x = TextOut(&appDevice,x,y," ",1);
    tBuf[0] = curTm->tm_mday/10+'0';
    tBuf[1] = curTm->tm_mday%10+'0';
    x = TextOut(&appDevice,x,y,tBuf,2);
    x = TextOut(&appDevice,x,y," ",1);
    x = TextOut(&appDevice,x,y,WeekText[curTm->tm_hour>12 ? 8 : 7],2);
//    TextOut_HighLight(&appDevice,x,y,tBuf+12,8);
//    TextOut(&appDevice,x,y+16,tBuf+5,6);
//    TextOut(&appDevice,x,y+32,tBuf+21,4);
//    TextOut(&appDevice,x+32,y+32,tBuf+20,1);
//    TextOut_HighLight(&appDevice,x+40,y+32,tBuf+1,3);
        // Battary check
        // Full charge is 4.2V, lower power is 3.3V
        // Vref = 3.28V
        // 10K/13.6K
        // 4.2*10/13.6/3.28*4096 = 3875
        // 3.3*10/13.6/3.28*4096 = 3030
#ifdef    DEBUG_BOARD
      {
        DrawIcon(104,0,ICON_ID_BATTARY);
        static u32 res1 = 16;
        if(res1)res1--;
        else res1=16;
        u32 res = res1 + 106;
        SetColor(BLACK);
        SetLineThickness(NORMAL_LINE);
        Bar(res,2,121,7);
      }
#else
      UpdateBatIcon(104,0,GetBatCap());
//      if(IsCHG()){
//        DrawIcon(104,0,ICON_ID_CHARGE);
//      }else if(IsPGOOD()){
//        DrawIcon(104,0,ICON_ID_POWER);
//      }else{
//        DrawIcon(104,0,ICON_ID_BATTARY);
//        u32 res = ADCResult.ADBat;
//        if(res > 3835){
//          res = 800;
//        }else if(res < 3030){
//          res = 0;
//        }else{
//          res -= 3030;
//        }
//        res = 16*res/800;
//        res += 106;
#endif
    }
    break;
  case MSG_KEY_UP:
    if((u32)msg->param >  SSD1303_FPS*2){
      deviceClose = 1;
      SSD1303_TurnOff();
      MMA_SLEEP();
    }else if(deviceClose){
      deviceClose = 0;
      SSD1303_TurnOn();
      MMA_WAKEUP();
    }else{
      PopupMenu(MainMenu);
      return MR_Finish;
    }
  }
  return MR_Continue;
}

MenuResult OnSetContrast(void* param, Msg* msg)
{
  if(msg->message == MSG_INIT){
    SetColor(BLACK);
    ClearDevice();
    InitialDevice(&appDevice,&SSD1303_Prop,fontBuffer_fixedsys);
    TextOut(&appDevice,0,0,UserText[UT_CHANGE_CONTRAST],0xFF);
    u32 con = SSD1303_GetContrast();
    char buf[] = {con/100+'0',(con/10)%10+'0',con%10+'0'};
    TextOut(&appDevice,45,18,buf,3);
    SetLineThickness(NORMAL_LINE);
    SetColor(WHITE);
    Rectangle(0,30,127,45);
    con = 123*con/256 + 2;
    Bar(2,32,con,43);
  }else if(msg->message == MSG_SCROLL){
    s16 enc = (s16)((s32)msg->param);
    u32 con = SSD1303_GetContrast();
    if(enc>0){
      if(con<255)con++;
    }else if(con){
      con--;
    }
    char buf[] = {con/100+'0',(con/10)%10+'0',con%10+'0'};
    TextOut(&appDevice,45,18,buf,3);
    SSD1303_SetContrast(con);
    SetColor(BLACK);
    Bar(2,32,125,43);
    SetColor(WHITE);
    con = 123*con/256 + 2;
    Bar(2,32,con,43);
  }else if(msg->message == MSG_KEY_UP){
    return MR_Finish;
  }
  return MR_Continue;
}

typedef unsigned long(*pfnTextOut)(Device*,Pos_t,Pos_t,const char*,Size_t);
void  UpdateText(pfnTextOut textOut,struct tm* pTM, u8 pos)
{
  u8 wid = appDevice.font->width;
  Pos_t yPos = 13;
  switch(pos){
  case 0:// Inital
    textOut(&appDevice,2+15*wid,yPos,":",1);
  case 1:// year
    {
    u32 year = pTM->tm_year + 1900;
    char buf[] = {year/1000+'0',year/100%10+'0',year/10%10+'0',year%10+'0'};
    textOut(&appDevice,2,yPos,buf,4);
    }
    if(pos)break;
  case 2:// Month
    textOut(&appDevice,2+5*wid,yPos,MonthText[pTM->tm_mon],3);
    if(pos)break;
  case 3:// Date
    {
    char buf[] = {pTM->tm_mday/10+'0',pTM->tm_mday%10+'0'};
    textOut(&appDevice,2+9*wid,yPos,buf,2);
    }
    if(pos)break;
  case 4:// Hour
    {
    char buf[] = {pTM->tm_hour/10+'0',pTM->tm_hour%10+'0'};
    textOut(&appDevice,2+13*wid,yPos,buf,2);
    }
    if(pos)break;
  case 5:// Minute
    {
    char buf[] = {pTM->tm_min/10+'0',pTM->tm_min%10+'0'};
    textOut(&appDevice,2+16*wid,yPos,buf,2);
    }
    if(pos)break;
  case 6:// OK
    textOut(&appDevice,2+1*wid,52,"OK",2);
    if(pos)break;
  case 7:// cancel
    textOut(&appDevice,126-7*wid,52,"Cancel",6);
    if(pos)break;
  default:
    break;
  }
}

#define   Round(param,low,high)   \
  if((param)<(low)){(param)=(high);}else if((param)>(high)){(param)=(low);}

void  ModifyTime(int enc, struct tm* pTM, int position)
{
      switch(position){
      case 1:
        pTM->tm_year += enc;
        Round(pTM->tm_year,-130,130);
        {
          u32 year = pTM->tm_year + 1900;
          char buf[] = {year/1000+'0',year/100%10+'0',year/10%10+'0',year%10+'0'};
          TextOut_HighLight(&appDevice,64,30,buf,4);
        }
        break;
      case 2:
        pTM->tm_mon += enc;
        Round(pTM->tm_mon,0,11);
        TextOut_HighLight(&appDevice,64,30,MonthText[pTM->tm_mon],3);
        break;
      case 3:
        pTM->tm_mday += enc;
        Round(pTM->tm_mday,1,31);
        {
          char buf[] = {pTM->tm_mday/10+'0',pTM->tm_mday%10+'0'};
          TextOut_HighLight(&appDevice,64,30,buf,2);
        }
        break;
      case 4:
        pTM->tm_hour += enc;
        Round(pTM->tm_hour,0,23);
        {
          char buf[] = {pTM->tm_hour/10+'0',pTM->tm_hour%10+'0'};
          TextOut_HighLight(&appDevice,64,30,buf,2);
        }
        break;
      case 5:
        pTM->tm_min += enc;
        Round(pTM->tm_min,0,59);
        {
          char buf[] = {pTM->tm_min/10+'0',pTM->tm_min%10+'0'};
          TextOut_HighLight(&appDevice,64,30,buf,2);
        }
        break;
      default:
        break;
      }
}

LPCSTR  SetTimeDesc_En[] = 
{
  "  Year:",
  " Month:",
  "  Date:",
  "  Hour:",
  "Minute:",
};

LPCSTR * SetTimeDesc  = SetTimeDesc_En;

MenuResult OnSetTime(void* param, Msg* msg)
{
  static  struct tm oldTm;
  static int position;
  static int lastPosition;
  static u8 modifyMode;
  if(msg->message == MSG_INIT){
    time_t now = (time_t)RTC_GetCounter();
    struct tm* ptm = localtime(&now);
    memcpy(&oldTm,ptm,sizeof(oldTm));
    lastPosition = 0xFF;
    position = 0;
    SetColor(BLACK);
    ClearDevice();
    InitialDevice(&appDevice,&SSD1303_Prop,fontBuffer_fixedsys);
    TextOut(&appDevice,0,0,UserText[UT_SET_TIME],0xFF);
    UpdateText(TextOut,&oldTm,position);
    position++;
    UpdateText(TextOut_HighLight,&oldTm,position);
    lastPosition = position;
    modifyMode = 0;
  }else if(msg->message == MSG_KEY_UP){
    if(position == 7){
      return MR_Finish;
    }else if(position == 6){
      oldTm.tm_sec = 0;
      RTC_SetCounter(mktime(&oldTm));
      RTC_WaitForLastTask();
      return MR_Finish;
    }else if(position<7 && position>0){
      if(modifyMode){
        modifyMode = 0;
        SetColor(BLACK);
        Bar(10,30,100,50);
        UpdateText(TextOut_HighLight,&oldTm,position);
      }else{
        modifyMode = 1;
        TextOut(&appDevice,20,30,SetTimeDesc[position-1],0xFF);
        ModifyTime(0,&oldTm,position);
      }
    }
  }else if(msg->message == MSG_SCROLL){
    int enc = (int)msg->param;
    if(enc>0)enc=1;
    else enc = -1;
    if(modifyMode){
      ModifyTime(enc,&oldTm,position);
    }else{
      position+=enc;
      if(position>=8){
        position = 1;
      }else if(position<1){
        position = 7;
      }
      UpdateText(TextOut_HighLight,&oldTm,position);
      UpdateText(TextOut,&oldTm,lastPosition);
      lastPosition = position;
    }
  }
  return MR_Continue;
}

MenuResult OnMeasureGravity(void* param, Msg* msg)
{
  if(msg->message == MSG_INIT){
    SetColor(BLACK);
    ClearDevice();
    InitialDevice(&appDevice,&SSD1303_Prop,fontBuffer_fixedsys);
    SetColor(WHITE);
    SetLineThickness(NORMAL_LINE);
    Rectangle(64,0,127,63);
    Line(0,63,63,63);
    Line(63,63,48,40);
    Line(16,40,48,40);
    Line(16,40,0,63);
    MMA_WAKEUP();
  }else if(msg->message == MSG_KEY_UP){
    MMA_SLEEP();
    return MR_Finish;
  }else if(msg->message == MSG_GRAV_XY){
    static u16 lastX = 64+64*2;
    static u16 lastY = 64/2;
    u16 gX = 4096-GetGravX(msg);
    u16 gY = 4096-GetGravY(msg);
    gX = gX*64 / 4096+64;
    gY = gY*64 / 4096;
    SetColor(BLACK);
    FillCircle(lastX,lastY,2);
    SetColor(WHITE);
    FillCircle(gX,gY,2);
    lastX = gX;
    lastY = gY;
  }else if(msg->message == MSG_GRAV_Z){
    static u16 lastZ = 50/2;
    u16 gZ = GetGravZ(msg);
    gZ = gZ*50 / 4096;
    SetColor(BLACK);
    Circle(32,lastZ,2);
    SetColor(WHITE);
    Circle(32,gZ,2);
    lastZ = gZ;
  }
  return MR_Continue;
}

MenuResult OnMeasureBat(void* param, Msg* msg)
{
  switch(msg->message){
  case MSG_INIT:
    SetColor(BLACK);
    ClearDevice();
    InitialDevice(&appDevice,&SSD1303_Prop,fontBuffer_fixedsys);
    TextOut(&appDevice,0,0,UserText[UT_VIEW_BATTARY],0xFF);
  case MSG_SECOND:
    if(IsCHG()){
      TextOut(&appDevice,10,16,UserText[UT_BAT_CHARGE],0xFF);
    }else if(IsPGOOD()){
      TextOut(&appDevice,10,16,UserText[UT_BAT_FULL],0xFF);
    }else{
      TextOut(&appDevice,10,16,UserText[UT_BAT_NORMAL],0xFF);
      //DrawIcon(104,0,ICON_ID_BATTARY);
    }
    {
      u32 res = GetBatCap();
      UpdateBatIcon(104,0,res);
      char buf[4] = {res/100 + '0',res/10%10+'0',res%10+'0','%'};
      Pos_t x = TextOut(&appDevice,10,32,UserText[UT_BAT_CAPACITY],0xFF);
      x = TextOut(&appDevice,x,32,buf,4);
      res = ADCResult.ADBat;
      res = res * 328 * 136 / (4096*100);
      char ADBuf[] = {(res/100)%10+'0', '.', 
      (res/10)%10+'0', res%10+'0','V', 0};
      x = TextOut(&appDevice,10,48,UserText[UT_BAT_VOLTAGE],0xFF);
      TextOut(&appDevice,x,48,ADBuf,5);
    }
    break;
  case MSG_KEY_UP:
    return MR_Finish;
  default:
    break;
  }
  return MR_Continue;
}
