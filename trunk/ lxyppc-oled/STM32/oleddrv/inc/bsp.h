#ifndef BSP_H
#define BSP_H

/**
  IO Connection map

       SSD1303
  Vcc   1  2  Gnd
D1/Data 3  4  D0/Clk
    D3  5  6  D2
    D5  7  8  D4
    D7  9 10  D6
    WR 11 12  RD
   RES 13 14  A0  (1-data,0-command)
   CS  15 16  C86 (1-8080, 0-SPI)

       STM32
  D0/Clk    PB13
  D1/Data   PB15
  RES       PB12
  A0        PB14
  CS        Gnd
  D2~D7     (Float)
  WR        (Float)
  RD        (Float)
 */

#define   SSD_Clk         GPIOB,GPIO_Pin_13
#define   SSD_Data        GPIOB,GPIO_Pin_15
#define   SSD_Reset       GPIOD,GPIO_Pin_12
#define   SSD_A0          GPIOD,GPIO_Pin_14
#define   SSD_CS          //GPIOG,GPIO_Pin_2

#define   SSD_Clk_Low()     GPIOB->BRR = GPIO_Pin_13
#define   SSD_Clk_High()    GPIOB->BSRR = GPIO_Pin_13
#define   SSD_Data_Low()    GPIOB->BRR = GPIO_Pin_15
#define   SSD_Data_High()   GPIOB->BSRR = GPIO_Pin_15
#define   SSD_Reset_Low()   GPIOB->BRR = GPIO_Pin_12
#define   SSD_Reset_High()  GPIOB->BSRR = GPIO_Pin_12
#define   SSD_A0_Low()      GPIOB->BRR = GPIO_Pin_14
#define   SSD_A0_High()     GPIOB->BSRR = GPIO_Pin_14
#define   SSD_CS_Low()      //GPIOG->BRR = GPIO_Pin_2
#define   SSD_CS_High()     //GPIOG->BSRR = GPIO_Pin_2

#endif
