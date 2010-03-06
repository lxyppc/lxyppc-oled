/*******************************************************************************
* File Name          : menu.h
* Author             : lxyppc
* Version            : V1.0
* Date               : 10-03-05
* Description        : menu header
*******************************************************************************/
#ifndef   MENU_H
#define   MENU_H

// Menu message definition
#define   MSG_SCROLL      0x00000001ul
#define   MSG_KEY_UP      0x00000002ul
#define   MSG_KEY_DOWN    0x00000003ul
#define   MSG_SECOND      0x00000004ul
#define   MSG_SENSE       0x00000005ul
#define   MSG_INIT        0x00000006ul
#define   MSG_POWER       0x00000007ul
#define   MSG_GRAV_XY     0x00000008ul
#define   MSG_GRAV_Z      0x00000009ul

#define   MakeMsgGrav_XY(pMsg,x,y)      \
  {\
    (pMsg)->message = MSG_GRAV_XY;\
    (pMsg)->param = (void*)((x) | (((unsigned long)(y))<<16));\
  }
#define   MakeMsgGrav_Z(pMsg,z)      \
  {\
    (pMsg)->message = MSG_GRAV_Z;\
    (pMsg)->param = (void*)(z);\
  }

// Here  Gravity X is MMA Y axis output
#define   GetGravX(pMsg)    (u16)(((u32)(pMsg)->param>>16) & 0x0FFF)
// Here  Gravity Y is MMA X axis output
#define   GetGravY(pMsg)    (u16)((u32)(pMsg)->param & 0x0FFF)
#define   GetGravZ(pMsg)    (u16)((u32)(pMsg)->param & 0x0FFF)

// POWER mode definition
#define   PWR_CHG         0x0001
#define   PWR_GOOD        0x0002
#define   GetBatValue(pMsg)     (u16)(((u32)(pMsg)->param)>>16)

// Menu type definition
#define   MT_NULL         0x00
#define   MT_SUB          0x01    // Has sub menu
#define   MT_CHECK        0x02    // Has check box

typedef   const char*    LPCSTR;

typedef   struct  _Msg
{
  void*           param;
  unsigned long   message;
}Msg;

typedef   enum{
  MR_Finish,
  MR_Error,
  MR_Continue,
}MenuResult;

typedef   MenuResult(*MenuFunc_t)(void* param, Msg* pVoid);

typedef   struct  _MenuItem
{
  const struct _MenuItem*   parent;
  union{
    const void*         justMakeInitialEasy;
    const MenuFunc_t          pfnMenu;
    const struct _MenuItem*   child;
  };
  unsigned char       index;    // menu index
  unsigned char       cnt;      // menu count
  unsigned char       res;      // menu resource
  unsigned  char      type;
}MenuItem;

MenuFunc_t    InitialMenu();

unsigned long  MenuProcess(Msg* msg);
MenuResult     PopupMenu(const MenuItem* menu);
unsigned long   GetMessage(Msg* msg);
unsigned long   PostMessage(Msg* msg);

#endif
