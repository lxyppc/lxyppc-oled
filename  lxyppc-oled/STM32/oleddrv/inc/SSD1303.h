#ifndef   SSD1303_H
#define   SSD1303_H
#include "DrawText.h"

void  SSD1303_Init(void);
extern  const DeviceProp  SSD1303_Prop;
unsigned long SSD1303_DrawBlock(Pos_t x, Pos_t y, Pos_t cx, Pos_t cy, const unsigned char* data);
#endif
