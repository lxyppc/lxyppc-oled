/*******************************************************************************
* File Name          : UserMenu.c
* Author             : lxyppc
* Version            : V1.0
* Date               : 10-03-05
* Description        : User menu implemention 
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
#include "StringResource.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
//#define     OnMeasureBat      DefaultMenuFunc
//#define     OnMeasureGravity  DefaultMenuFunc
//#define     OnSetContrast     DefaultMenuFunc
//#define     OnSetTime         DefaultMenuFunc
//#define     OnLanguageSet     DefaultMenuFunc
/* Private variables ---------------------------------------------------------*/
extern  const MenuItem    MainMenu[];
extern  const MenuItem    SettingMenu[];
extern  const MenuItem    StatusMenu[];
extern  const MenuItem    LanguageMenu[];
const LPCSTR* curLang = StringTable_CHS;
Device    appDevice;
extern  struct{
  u16   ADBat;
  u16   ADX;
  u16   ADY;
  u16   ADZ;
}ADCResult;

#define   LoadString(STR_ID)      curLang[(STR_ID)]
// January is 0
#define   LoadStrMonth(Month)     LoadString(STR_JAN + (Month))
// Sunday is 0
#define   LoadStrWeek(WeekDay)    LoadString(STR_SUNDAY + (WeekDay))
// Year is 0, then month,date,hour and minute
#define   LoadStrTimeDesc(index)  LoadString(STR_YEAR + (index))

//LPCSTR* MonthText = MonthText_En;
//LPCSTR* WeekText = WeekText_En;
//LPCSTR * SetTimeDesc  = SetTimeDesc_En;
//LPCSTR* UserText = UserText_En;
//#define   UserText    (curLang)
//#define   MonthText   (curLang + STR_JAN)
//#define   WeekText    (curLang + STR_SUNDAY)
//#define   SetTimeDesc (curLang + STR_YEAR)

/* Private function prototypes -----------------------------------------------*/
MenuResult OnMenuAbout(void* p, Msg* msg);
MenuResult OnMenuTest(void* p, Msg* msg);
MenuResult DefaultMenuFunc(void* p, Msg* msg);
MenuResult OnMenuRoot(void* param, Msg* msg);
MenuResult OnSetContrast(void* param, Msg* msg);
MenuResult OnSetTime(void* param, Msg* msg);
MenuResult OnMeasureGravity(void* param, Msg* msg);
MenuResult OnMeasureBat(void* param, Msg* msg);
MenuResult OnLanguageSet(void* param, Msg* msg);

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/************************** Menu definitions begin ****************************/
/* Root main */
const MenuItem    RootMenu = {
        /*Parent    child/pfn           index      cnt        resource        type*/
            0,      (void*)OnMenuRoot,         0,        1,        STR_ROOT,    MT_NULL
};
/* Main menu */
const MenuItem    MainMenu[] = {
      /*Parent    child/pfn           index      cnt        resource        type*/
  {         0,     StatusMenu,         0,        4,         STR_STATUS,     MT_SUB},
  {         0,     (void*)OnMenuTest,   1,        4,        STR_TEST,       MT_NULL},
  {         0,     SettingMenu,         2,        4,        STR_SETTING,    MT_SUB},
  {         0,     (void*)OnMenuAbout,  3,        4,        STR_ABOUT,      MT_NULL},
};

/* Setting menu */
const MenuItem    SettingMenu[] = {
      /*Parent    child/pfn                 index      cnt        resource        type*/
  {   MainMenu+2,   (void*)OnSetContrast,     0,        3,  STR_SETTING_CONTRAST,    MT_NULL},
  {   MainMenu+2,   (void*)OnSetTime,         1,        3,  STR_SETTING_TIME,        MT_NULL},
  {   MainMenu+2,   LanguageMenu,             2,        3,  STR_SETTING_LANGUAGE,    MT_SUB},
};
/* Language menu*/
const MenuItem    LanguageMenu[] = {
      /*Parent    child/pfn                 index      cnt        resource        type*/
  {SettingMenu+2,   (void*)OnLanguageSet,     0,        2,        STR_LANG_ENG,   MT_NULL},
  {SettingMenu+2,   (void*)OnLanguageSet,     1,        2,        STR_LANG_CHS,   MT_NULL},
};

/**/
const MenuItem    StatusMenu[] = {
  /*Parent          child/pfn               index      cnt        resource        type*/
  {   MainMenu+0,   (void*)OnMeasureBat,      0,        2,        STR_STATUS_BAT,     MT_NULL},
  {   MainMenu+0,   (void*)OnMeasureGravity,  1,        2,        STR_STATUS_GRAVITY, MT_NULL},
};
/************************** Menu definitions end   ****************************/
/******************************************************************************/

/*******************************************************************************
* Function Name  : OnMenuAbout
* Description    : Callback function for about menu
* Input          : void*  pointer of the menu item when message is MSG_INIT
* Output         : Msg*   pointer of the msg structure
* Return         : None
*******************************************************************************/
MenuResult OnMenuAbout(void* p, Msg* msg)
{
  if(msg->message == MSG_INIT){
    SetColor(BLACK);
    ClearDevice();
    TextOut(&appDevice, 0,  5,LoadString(STR_ABOUT_0), 0xFF);
    TextOut(&appDevice, 0, 25,LoadString(STR_ABOUT_1), 0xFF);
    TextOut(&appDevice, 0, 45,LoadString(STR_ABOUT_2), 0xFF);
    return MR_Continue;
  }else if(msg->message == MSG_KEY_UP){
    return MR_Finish;
  }
  return MR_Continue;
}

/*******************************************************************************
* Function Name  : OnMenuTest
* Description    : Callback function for test menu
* Input          : void*  pointer of the menu item when message is MSG_INIT
* Output         : Msg*   pointer of the msg structure
* Return         : None
*******************************************************************************/
MenuResult OnMenuTest(void* p, Msg* msg)
{
  if(msg->message == MSG_INIT){
    SetColor(BLACK);
    ClearDevice();
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

/*******************************************************************************
* Function Name  : DefaultMenuFunc
* Description    : Default menu callback function
* Input          : void*  pointer of the menu item when message is MSG_INIT
* Output         : Msg*   pointer of the msg structure
* Return         : None
*******************************************************************************/
MenuResult DefaultMenuFunc(void* p, Msg* msg)
{
  if(msg->message == MSG_INIT){
    SetColor(BLACK);
    ClearDevice();
    TextOut(&appDevice, 0, 25,curLang[STR_DEFAULT], 0xFF);
    return MR_Continue;
  }else if(msg->message == MSG_KEY_UP){
    return MR_Finish;
  }
  return MR_Continue;
}

/*******************************************************************************
* Function Name  : GetBatCap
* Description    : Get battery capacity
* Input          : None
* Output         : Battary capacity percentage
* Return         : None
* Details        : Battary fully charge is 4.2V, and lower power is 3.3V
                   Vref = 3.28V, resistive partial-pressure is 10K/13.6K
                   In 12 bit ADC, 4.2*10/13.6/3.28*4096 = 3875
                                  3.3*10/13.6/3.28*4096 = 3030
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : UpdateBatIcon
* Description    : Update the battery icon
* Input          : Pos_t  x position of the icon
*                  Pos_t  y position of the icon
*                  u32    battery capacity, in percentage
* Output         : Msg*   pointer of the msg structure
* Return         : None
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : OnMenuRoot
* Description    : Root menu callback function
* Input          : void*  pointer of the menu item when message is MSG_INIT
* Output         : Msg*   pointer of the msg structure
* Return         : None
*******************************************************************************/
MenuResult OnMenuRoot(void* param, Msg* msg)
{
  static  u8  deviceClose = 0;
  switch(msg->message){
  case MSG_INIT:
    SetColor(BLACK);
    ClearDevice();
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
    x = TextOut(&appDevice,x,y,LoadStrWeek(curTm->tm_wday),3);
    x = 70;
    y += 13;
    x = TextOut(&appDevice,x,y,LoadStrMonth(curTm->tm_mon),0xFF);
    x = TextOut(&appDevice,x,y," ",1);
    tBuf[0] = curTm->tm_mday/10+'0';
    tBuf[1] = curTm->tm_mday%10+'0';
    x = TextOut(&appDevice,x,y,tBuf,2);
    x = TextOut(&appDevice,x,y," ",1);
    x = TextOut(&appDevice,70,y-26,LoadString(curTm->tm_hour>12 ? STR_PM : STR_AM),2);

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

/*******************************************************************************
* Function Name  : OnSetContrast
* Description    : Set contrast menu callback function
* Input          : void*  pointer of the menu item when message is MSG_INIT
* Output         : Msg*   pointer of the msg structure
* Return         : None
*******************************************************************************/
MenuResult OnSetContrast(void* param, Msg* msg)
{
  if(msg->message == MSG_INIT){
    SetColor(BLACK);
    ClearDevice();
    TextOut(&appDevice,0,0,LoadString(STR_CHANGE_CONTRAST),0xFF);
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
#define   DrawOK()            UpdateTimeText(TextOut,0, TI_OK)
#define   DrawCancel()        UpdateTimeText(TextOut,0, TI_Cancel)
#define   HighLightOK()       UpdateTimeText(TextOut_HighLight,0, TI_OK)
#define   HighLightCancel()   UpdateTimeText(TextOut_HighLight,0, TI_Cancel)
typedef   enum{
  TI_All = 0,
  TI_Year = 1,
  TI_First = 1,
  TI_Month = 2,
  TI_Date = 3,
  TI_Hour = 4,
  TI_Minute = 5,
  TI_OK = 6,
  TI_Cancel = 7,
  TI_Last = 8,
  TI_Unknown = 0xFF,
}TimeIndex;
/*******************************************************************************
* Function Name  : UpdateTimeText
* Description    : Update texts when set time
* Input          : pfnTextOut   pointer of textOut Function
*                  struct tm*   time structure to display
*                  TimeIndex    time item index
* Output         : Msg*   pointer of the msg structure
* Return         : None
*******************************************************************************/
void  UpdateTimeText(pfnTextOut textOut,struct tm* pTM, TimeIndex timeIndex)
{
  u8 wid = GetFontTextWidth(appDevice.pfnFont,' ');
  Pos_t yPos = 13;
  switch(timeIndex){
  case TI_All:// Inital
    textOut(&appDevice,2+16*wid,yPos,":",1);
  case TI_Year:// year
    {
    u32 year = pTM->tm_year + 1900;
    char buf[] = {year/1000+'0',year/100%10+'0',year/10%10+'0',year%10+'0'};
    textOut(&appDevice,2,yPos,buf,4);
    }
    if(timeIndex!=TI_All)break;
  case TI_Month:// Month
    textOut(&appDevice,2+5*wid,yPos,LoadStrMonth(pTM->tm_mon),3);
    if(timeIndex!=TI_All)break;
  case TI_Date:// Date
    {
    char buf[] = {pTM->tm_mday/10+'0',pTM->tm_mday%10+'0'};
    textOut(&appDevice,2+10*wid,yPos,buf,2);
    }
    if(timeIndex!=TI_All)break;
  case TI_Hour:// Hour
    {
    char buf[] = {pTM->tm_hour/10+'0',pTM->tm_hour%10+'0'};
    textOut(&appDevice,2+14*wid,yPos,buf,2);
    }
    if(timeIndex!=TI_All)break;
  case TI_Minute:// Minute
    {
    char buf[] = {pTM->tm_min/10+'0',pTM->tm_min%10+'0'};
    textOut(&appDevice,2+17*wid,yPos,buf,2);
    }
    if(timeIndex!=TI_All)break;
  case TI_OK:// OK
    textOut(&appDevice,2+1*wid,52,LoadString(STR_OK),2);
    if(timeIndex!=TI_All)break;
  case TI_Cancel:// cancel
    textOut(&appDevice,126-7*wid,52,LoadString(STR_CANCEL),6);
    if(timeIndex!=TI_All)break;
  default:
    break;
  }
}

#define   Round(param,low,high)   \
  if((param)<(low)){(param)=(high);}else if((param)>(high)){(param)=(low);}
/*******************************************************************************
* Function Name  : ModifyTime
* Description    : Set contrast menu callback function
* Input          : int          Current encoder roll dirction
*                  struct tm*   time structure to be modified
*                  TimeIndex    time item index          
* Output         : Msg*   pointer of the msg structure
* Return         : None
*******************************************************************************/
void  ModifyTime(int enc, struct tm* pTM, TimeIndex timeIndex)
{
      switch(timeIndex){
      case TI_Year:
        pTM->tm_year += enc;
        Round(pTM->tm_year,-130,130);
        {
          u32 year = pTM->tm_year + 1900;
          char buf[] = {year/1000+'0',year/100%10+'0',year/10%10+'0',year%10+'0'};
          TextOut_HighLight(&appDevice,64,30,buf,4);
        }
        break;
      case TI_Month:
        pTM->tm_mon += enc;
        Round(pTM->tm_mon,0,11);
        TextOut_HighLight(&appDevice,64,30,LoadStrMonth(pTM->tm_mon),3);
        break;
      case TI_Date:
        pTM->tm_mday += enc;
        Round(pTM->tm_mday,1,31);
        {
          char buf[] = {pTM->tm_mday/10+'0',pTM->tm_mday%10+'0'};
          TextOut_HighLight(&appDevice,64,30,buf,2);
        }
        break;
      case TI_Hour:
        pTM->tm_hour += enc;
        Round(pTM->tm_hour,0,23);
        {
          char buf[] = {pTM->tm_hour/10+'0',pTM->tm_hour%10+'0'};
          TextOut_HighLight(&appDevice,64,30,buf,2);
        }
        break;
      case TI_Minute:
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

/*******************************************************************************
* Function Name  : OnSetTime
* Description    : Set time menu callback function
* Input          : void*  pointer of the menu item when message is MSG_INIT
* Output         : Msg*   pointer of the msg structure
* Return         : None
*******************************************************************************/
MenuResult OnSetTime(void* param, Msg* msg)
{
  static  struct tm oldTm;
  static TimeIndex position;
  static TimeIndex lastPosition;
  static u8 modifyMode;
  if(msg->message == MSG_INIT){
    time_t now = (time_t)RTC_GetCounter();
    struct tm* ptm = localtime(&now);
    memcpy(&oldTm,ptm,sizeof(oldTm));
    lastPosition = TI_Unknown;
    position = TI_All;
    SetColor(BLACK);
    ClearDevice();
    TextOut(&appDevice,0,0,LoadString(STR_SET_TIME),0xFF);
    UpdateTimeText(TextOut,&oldTm,position);
    position++;
    UpdateTimeText(TextOut_HighLight,&oldTm,position);
    lastPosition = position;
    modifyMode = 0;
  }else if(msg->message == MSG_KEY_UP){
    if(position == TI_Cancel){
      return MR_Finish;
    }else if(position == TI_OK){
      oldTm.tm_sec = 0;
      RTC_SetCounter(mktime(&oldTm));
      RTC_WaitForLastTask();
      return MR_Finish;
    }else if(position<7 && position>0){
      if(modifyMode){
        modifyMode = 0;
        SetColor(BLACK);
        Bar(10,30,100,50);
        UpdateTimeText(TextOut_HighLight,&oldTm,position);
      }else{
        modifyMode = 1;
        TextOut(&appDevice,20,30,LoadStrTimeDesc(position-1),0xFF);
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
      if(position>=TI_Last){
        position = TI_Year;
      }else if(position<TI_First){
        position = TI_Cancel;
      }
      UpdateTimeText(TextOut_HighLight,&oldTm,position);
      UpdateTimeText(TextOut,&oldTm,lastPosition);
      lastPosition = position;
    }
  }
  return MR_Continue;
}

/*******************************************************************************
* Function Name  : OnMeasureGravity
* Description    : Measure gravity menu callback function
* Input          : void*  pointer of the menu item when message is MSG_INIT
* Output         : Msg*   pointer of the msg structure
* Return         : None
*******************************************************************************/
MenuResult OnMeasureGravity(void* param, Msg* msg)
{
  if(msg->message == MSG_INIT){
    SetColor(BLACK);
    ClearDevice();
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

/*******************************************************************************
* Function Name  : OnMeasureBat
* Description    : Measure battery menu callback function
* Input          : void*  pointer of the menu item when message is MSG_INIT
* Output         : Msg*   pointer of the msg structure
* Return         : None
*******************************************************************************/
MenuResult OnMeasureBat(void* param, Msg* msg)
{
  switch(msg->message){
  case MSG_INIT:
    SetColor(BLACK);
    ClearDevice();
    TextOut(&appDevice,0,0,LoadString(STR_VIEW_BATTARY),0xFF);
  case MSG_SECOND:
    if(IsCHG()){
      TextOut(&appDevice,10,16,LoadString(STR_BAT_CHARGE),0xFF);
    }else if(IsPGOOD()){
      TextOut(&appDevice,10,16,LoadString(STR_BAT_FULL),0xFF);
    }else{
      TextOut(&appDevice,10,16,LoadString(STR_BAT_NORMAL),0xFF);
      //DrawIcon(104,0,ICON_ID_BATTARY);
    }
    {
      u32 res = GetBatCap();
      UpdateBatIcon(104,0,res);
      char buf[4] = {res/100 + '0',res/10%10+'0',res%10+'0','%'};
      Pos_t x = TextOut(&appDevice,10,32,LoadString(STR_BAT_CAPACITY),0xFF);
      x = TextOut(&appDevice,x,32,buf,4);
      res = ADCResult.ADBat;
      res = res * 328 * 136 / (4096*100);
      char ADBuf[] = {(res/100)%10+'0', '.', 
      (res/10)%10+'0', res%10+'0','V', 0};
      x = TextOut(&appDevice,10,48,LoadString(STR_BAT_VOLTAGE),0xFF);
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

/*******************************************************************************
* Function Name  : OnLanguageSet
* Description    : Set language menu callback function
* Input          : void*  pointer of the menu item when message is MSG_INIT
* Output         : Msg*   pointer of the msg structure
* Return         : None
*******************************************************************************/
MenuResult OnLanguageSet(void* param, Msg* msg)
{
  static const LPCSTR* preLang = NULL;
  static  u8 curSel = 0;
  switch(msg->message){
  case MSG_INIT:
  {
    SetColor(BLACK);
    ClearDevice();
    switch(((const MenuItem*)param)->res){
    case STR_LANG_ENG:
      preLang = StringTable_ENG;
      TextOut(&appDevice,30,20,preLang[STR_LANG_ENG],0xFF);
      break;
    case STR_LANG_CHS:
    default:
      preLang = StringTable_CHS;
      TextOut(&appDevice,30,20,preLang[STR_LANG_CHS],0xFF);
      break;
    }
    curSel = 1;
  }
  case MSG_SCROLL:
    {
      curSel = !curSel;
      const LPCSTR* tmp = curLang;
      curLang = preLang;
      if(curSel){
        HighLightOK();
        DrawCancel();
      }else{
        DrawOK();
        HighLightCancel();
      }
      curLang = tmp;
    }
    break;
  case MSG_KEY_UP:
    if(curSel){
      curLang = preLang;
    }
    return MR_Finish;
  }
  
  return MR_Continue;
}
