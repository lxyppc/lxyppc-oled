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

// Menu type definition
#define   MT_NULL         0x00
#define   MT_SUB          0x01    // Has sub menu
#define   MT_CHECK        0x02    // Has check box

typedef   struct  _Msg
{
  void*           param;
  unsigned long   message;
}Msg;

typedef   unsigned long(*MenuFunc_t)(void* param, Msg* pVoid);

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

unsigned long   GetMessage(Msg* msg);
unsigned long   PostMessage(Msg* msg);

#endif
