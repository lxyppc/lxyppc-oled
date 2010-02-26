/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_desc.c
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : Descriptors for Custom HID Demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "bootloader.h"
#include "config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* USB Standard Device Descriptor */
const u8 USERHID_DeviceDescriptor[USERHID_SIZ_DEVICE_DESC] =
  {
    0x12,                       /*bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x00,                       /*bcdUSB */
    0x02,
    0x00,                       /*bDeviceClass*/
    0x00,                       /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    0x40,                       /*bMaxPacketSize40*/
    (u8)LXYPPC_VID,             /*idVendor (0x1985)*/
    (u8)(LXYPPC_VID>>8),
    (u8)LXYPPC_PID,             /*idProduct = 0x1017*/
    (u8)(LXYPPC_PID>>8),
    (u8)DEV_VERSION,            /*bcdDevice rel.  2.60*/
    (u8)(DEV_VERSION>>8),
    1,                          /*Index of string descriptor describing
                                              manufacturer */
    2,                          /*Index of string descriptor describing
                                             product*/
    3,                          /*Index of string descriptor describing the
                                             device serial number */
    0x01                        /*bNumConfigurations*/
  }
  ; /* USERHID_DeviceDescriptor */


/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
u8 USERHID_ConfigDescriptor[USERHID_SIZ_CONFIG_DESC] =
  {
    0x09, /* bLength: Configuation Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    USERHID_SIZ_CONFIG_DESC,
    /* wTotalLength: Bytes returned */
    0x00,
    0x01,         /*bNumInterfaces: 1 interface*/
    0x01,         /*bConfigurationValue: Configuration value*/
    0x00,         /*iConfiguration: Index of string descriptor describing
                                 the configuration*/
    0xC0,         /* Bus powered no wake up, bmAttributes: self powered */
    0xFA,         /*MaxPower 500 mA: this current is used for detecting Vbus*/
    /* 09 */

/////////////////////////////////////////////////////////////////////////////    
    /************** Descriptor of interface 1 ****************/
    0x09,         /*bLength: Interface Descriptor size*/
    USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
    0x00,         /*bInterfaceNumber: Number of Interface*/
    0x00,         /*bAlternateSetting: Alternate setting*/
    0x02,         /*bNumEndpoints*/
    0x03,         /*bInterfaceClass: HID*/
    0x00, //0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
    0x00, //0x02,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
    0,            /*iInterface: Index of string descriptor*/
    /******************** Descriptor of Joystick Mouse HID ********************/
    /* 18 */
    0x09,         /*bLength: HID Descriptor size*/
    HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
    0x10,//0x00,         /*bcdHID: HID Class Spec release number*/
    0x01,
    0x00,         /*bCountryCode: Hardware target country*/
    0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
    0x22,         /*bDescriptorType*/
    INT1_SIZ_REPORT_DESC,/*wItemLength: Total length of Report descriptor*/
    0x00,
    /******************** Descriptor of Joystick Mouse endpoint ********************/
    /* 27 */
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
    0x81,          /*bEndpointAddress: Endpoint Address (IN)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    0x40, //0x04,          /*wMaxPacketSize: 120 Byte max */
    0x00,
    0x01,          /*bInterval: Polling Interval (60 ms)*/
      /* 34 */      
            //OUT endpoint
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
    0x01,          /*bEndpointAddress: Endpoint Address (OUT)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    0x40, //0x04,          /*wMaxPacketSize: 120 Byte max */
    0x00,
    0x01,          /*bInterval: Polling Interval (60 ms)*/
    /*41*/
  };

// report descriptor for Interface 1
const u8 INT1_ReportDescriptor[INT1_SIZ_REPORT_DESC] =
{
    0x05, 0x97,                    // USAGE_PAGE (lxyppc Prod)
    0x09, 0x01,                    // USAGE (OledProject)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x01,                    //   REPORT_ID (1)
    0x09, 0x02,                    //   USAGE (DefaultIn)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x3f,                    //   REPORT_COUNT (63)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x09, 0x03,                    //   USAGE (DefaultOut)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x7f,              //   LOGICAL_MAXIMUM (32767)
    0x09, 0x06,                    //   USAGE (FlashSizeKBytes)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x07,                    //   REPORT_COUNT (7)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0x85, 0x02,                    //   REPORT_ID (2)
    0x09, 0x08,                    //   USAGE (WorkMode)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};

/* USB String Descriptors (optional) */
const u8 USERHID_StringLangID[USERHID_SIZ_STRING_LANGID] =
  {
    USERHID_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04
  }
  ; /* LangID = 0x0409: U.S. English */



const u8 USERHID_StringVendor[USERHID_SIZ_STRING_VENDOR] =
  {
    USERHID_SIZ_STRING_VENDOR, /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
    /* Manufacturer: "lxyppc" */
    'l', 0, 'x', 0, 'y', 0, 'p', 0, 'p', 0, 'c', 0,
    
  };


const u8 USERHID_StringProduct[USERHID_SIZ_STRING_PRODUCT] =
  {
    USERHID_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    /* lxyppc oled project */
    'l', 0, 'x', 0, 'y', 0, 'p', 0, 'p', 0, 'c', 0, ' ', 0, 
    'o', 0, 'l', 0, 'e', 0, 'd', 0, ' ', 0, 'p', 0, 'r', 0, 
    'o', 0, 'j', 0, 'e', 0, 'c', 0, 't', 0,
  };
u8 USERHID_StringSerial[USERHID_SIZ_STRING_SERIAL] =
  {
    USERHID_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'I',0, 'R',0,//'S',0,'S',0,'S',0,
  };
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

