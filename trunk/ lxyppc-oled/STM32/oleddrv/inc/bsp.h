#ifndef BSP_H
#define BSP_H

/**
  IO Connection map

       SSD1303                                 STM32

  Vcc   1  2  Gnd                         Vcc   1  2   Gnd
D1/Data 3  4  D0/Clk                     PB12   3  4   PB13
    D3  5  6  D2                         PB14   5  6   PB15
    D5  7  8  D4                         PD8    7  8   PD9
    D7  9 10  D6                         PD10   9  10  PD11
    WR 11 12  RD                         PD12  11  12  PD13
   RES 13 14  A0  (1-data,0-command)     PD14  13  14  PD15
   CS  15 16  C86 (1-8080, 0-SPI)        PG2   15  16  PG3
 */

#define   SSD_Clk         GPIOB,GPIO_Pin_12
#define   SSD_Data        GPIOB,GPIO_Pin_13
#define   SSD_Reset       GPIOD,GPIO_Pin_14
#define   SSD_A0          GPIOD,GPIO_Pin_15
#define   SSD_CS          GPIOG,GPIO_Pin_2

#define   SSD_Clk_Low()     GPIOB->BRR = GPIO_Pin_13
#define   SSD_Clk_High()    GPIOB->BSRR = GPIO_Pin_13
#define   SSD_Data_Low()    GPIOB->BRR = GPIO_Pin_12
#define   SSD_Data_High()   GPIOB->BSRR = GPIO_Pin_12
#define   SSD_Reset_Low()   GPIOD->BRR = GPIO_Pin_14
#define   SSD_Reset_High()  GPIOD->BSRR = GPIO_Pin_14
#define   SSD_A0_Low()      GPIOD->BRR = GPIO_Pin_15
#define   SSD_A0_High()     GPIOD->BSRR = GPIO_Pin_15
#define   SSD_CS_Low()      GPIOG->BRR = GPIO_Pin_2
#define   SSD_CS_High()     GPIOG->BSRR = GPIO_Pin_2

#endif
