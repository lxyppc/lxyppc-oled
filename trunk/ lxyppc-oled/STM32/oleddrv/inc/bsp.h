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
  D0/Clk    PB13/SPI2 Clk
  D1/Data   PB15/SPI2 MOSI
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

#define   SPI_SendByte(data)  SPI2->DR = (data)
#define   SPI_Wait()           while(!(SPI2->SR&SPI_I2S_FLAG_TXE));while(SPI2->SR&SPI_I2S_FLAG_BSY);

#define   SSD1303_FPS                   50



#define   IsLedOn()       (!(GPIOA->ODR & GPIO_Pin_8))
#define   LED_ON()        GPIOA->BRR = GPIO_Pin_8
#define   LED_OFF()       GPIOA->BSRR = GPIO_Pin_8
#define   ToggleLED()     if(GPIOA->ODR & GPIO_Pin_8){GPIOA->BRR = GPIO_Pin_8;}\
                          else{GPIOA->BSRR = GPIO_Pin_8;}

#define   IsCHG()         (!(GPIOB->IDR & GPIO_Pin_5))
#define   IsPGOOD()       (!(GPIOB->IDR & GPIO_Pin_4))

#define   GSel1_High()    GPIOB->BSRR = GPIO_Pin_9
#define   GSel1_Low()     GPIOB->BRR = GPIO_Pin_9
#define   GSel2_High()    GPIOB->BSRR = GPIO_Pin_8
#define   GSel2_Low()     GPIOB->BRR = GPIO_Pin_8

#define   MMA_SLEEP()     GPIOB->BRR = GPIO_Pin_11
#define   MMA_WAKEUP()    GPIOB->BSRR = GPIO_Pin_11

#define   AD_CH_BAT       ADC_Channel_4
#define   AD_CH_X         ADC_Channel_1
#define   AD_CH_Y         ADC_Channel_2
#define   AD_CH_Z         ADC_Channel_3
#define   AD_CH_CHG       ADC_Channel_9

#define   DMA_SSD_1303    DMA1_Channel5
#define   DMA_Handler_SSD_1303    DMA1_Channel5_IRQHandler
#define   DMA_ADC         DMA1_Channel1

#endif
