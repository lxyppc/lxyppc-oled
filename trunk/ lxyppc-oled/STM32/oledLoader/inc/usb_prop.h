/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_prop.h
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : All processings related to Custom HID demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_PROP_H
#define __USB_PROP_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum _HID_REQUESTS
{
  GET_REPORT = 1,
  GET_IDLE,
  GET_PROTOCOL,

  SET_REPORT = 9,
  SET_IDLE,
  SET_PROTOCOL
} HID_REQUESTS;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void USERHID_init(void);
void USERHID_Reset(void);
void USERHID_SetConfiguration(void);
void USERHID_GetConfiguration(void);
void USERHID_SetDeviceAddress (void);
void USERHID_Status_In (void);
void USERHID_Status_Out (void);
RESULT USERHID_Data_Setup(u8);
RESULT USERHID_NoData_Setup(u8);
RESULT USERHID_Get_Interface_Setting(u8 Interface, u8 AlternateSetting);
u8 *USERHID_GetDeviceDescriptor(u16 );
u8 *USERHID_GetConfigDescriptor(u16);
u8 *USERHID_GetStringDescriptor(u16);
RESULT USERHID_SetProtocol(void);
u8 *USERHID_GetProtocolValue(u16 Length);
RESULT USERHID_SetIdle(void);
u8 *USERHID_GetIdleValue(u16 Length);
u8* USERHID_SetReportFeature(u16 Length);
RESULT USERHID_SetProtocol(void);

u8 *INT1_GetReportDescriptor(u16 Length);
u8 *INT1_GetHIDDescriptor(u16 Length);
u8 *INT2_GetReportDescriptor(u16 Length);
u8 *INT2_GetHIDDescriptor(u16 Length);
u8 *INT3_GetReportDescriptor(u16 Length);
u8 *INT3_GetHIDDescriptor(u16 Length);

void USERHID_GetStatus(void);
/* Exported define -----------------------------------------------------------*/
//#define USERHID_GetConfiguration          NOP_Process
//#define USERHID_SetConfiguration          NOP_Process
#define USERHID_GetInterface              NOP_Process
#define USERHID_SetInterface              NOP_Process
//#define USERHID_GetStatus                 NOP_Process
#define USERHID_ClearFeature              NOP_Process
#define USERHID_SetEndPointFeature        NOP_Process
#define USERHID_SetDeviceFeature          NOP_Process
//#define USERHID_SetDeviceAddress          NOP_Process

#define REPORT_DESCRIPTOR                  0x22

#define GS_REPORT_INPUT                     0x01
#define GS_REPORT_OUTPUT                    0x02
#define GS_REPORT_FEATURE                   0x03

#endif /* __USB_PROP_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
