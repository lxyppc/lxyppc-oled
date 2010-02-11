/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_desc.h
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : Descriptor Header for Custom HID Demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define HID_DESCRIPTOR_TYPE                     0x21
#define INT1_SIZ_HID_DESC                       0x09
#define INT1_OFF_HID_DESC                       0x12
#define INT2_OFF_HID_DESC                       41
#define INT2_SIZ_HID_DESC                       9
#define INT3_OFF_HID_DESC                       66
#define INT3_SIZ_HID_DESC                       9

#define INT1_SIZ_REPORT_DESC                    49
#define INT2_SIZ_REPORT_DESC                    63
#define INT3_SIZ_REPORT_DESC                    ( 65 + 61 )

#define USERHID_SIZ_DEVICE_DESC                     18
#define USERHID_SIZ_CONFIG_DESC                     41//107 //91 //66//81 //49 //41 //34

#define USERHID_SIZ_STRING_LANGID                   4
#define USERHID_SIZ_STRING_VENDOR                   16
#define USERHID_SIZ_STRING_PRODUCT                  50
#define USERHID_SIZ_STRING_SERIAL                   32
#define USERHID_SIZ_STRING_VERSION                  32

#define STANDARD_ENDPOINT_DESC_SIZE             0x09

/* Exported functions ------------------------------------------------------- */
extern const u8 USERHID_DeviceDescriptor[USERHID_SIZ_DEVICE_DESC];
extern u8 USERHID_ConfigDescriptor[USERHID_SIZ_CONFIG_DESC];
extern const u8 INT1_ReportDescriptor[INT1_SIZ_REPORT_DESC];
extern const u8 INT2_ReportDescriptor[INT2_SIZ_REPORT_DESC];
extern const u8 INT3_ReportDescriptor[INT3_SIZ_REPORT_DESC];
extern const u8 USERHID_StringLangID[USERHID_SIZ_STRING_LANGID];
extern const u8 USERHID_StringVendor[USERHID_SIZ_STRING_VENDOR];
extern const u8 USERHID_StringProduct[USERHID_SIZ_STRING_PRODUCT];
extern u8 USERHID_StringSerial[USERHID_SIZ_STRING_SERIAL];
extern u8 USERHID_StringVersion[USERHID_SIZ_STRING_VERSION];

#endif /* __USB_DESC_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
