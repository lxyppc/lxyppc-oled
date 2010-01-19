#ifndef   SSD1303_H
#define   SSD1303_H

/**
  The SSH1101A is compatible with SSD1303
 */

void WriteCommand(unsigned char command);
void WriteData(unsigned char data);
void SSD1303_Init(void);

#endif
