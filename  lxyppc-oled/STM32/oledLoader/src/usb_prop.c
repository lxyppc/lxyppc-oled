/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_prop.c
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : All processings related to Custom HID Demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"
#include "config.h"
#include "Bootloader.h"
///////////////////////////////////////////////
///  Data for idle and protocol
///
typedef struct _IDLE_AND_PROTOCOL
{
  unsigned char   idle[3];           /// Idle duration for interface 0,1,2
  unsigned char   reserved0;
  unsigned char   protocol[3];       /// device current protocol for interface 0,1,2
  unsigned char   reserved1;
}IDLE_AND_PROTOCOL, *PIDLE_AND_PROTOCOL;


#define   GET_REPORT_INPUT        1
#define   GET_REPORT_OUTPUT       2
#define   GET_REPORT_FEATURE      3

#define   MAKE_ID_TYPE(id,type)   \
  ((((unsigned short)(id))<<8) | (unsigned short)(type))

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//u32 ProtocolValue;
/// Default idle rate 0, default protocol 1(report protocol)
IDLE_AND_PROTOCOL   idleAndProtocol = {{0,0,0},0,{1,1,1},0};
extern DeviceFeature   deviceFeature;
#define   deviceFeatureBuffer     ((u8*)&deviceFeature)
#define   deviceModeBuffer        ((u8*)&deviceFeature.workMode)

u8 *GetReportCopyRoutine(u16 Length);
u8* UserCopyRoutine(u16 Length);
extern u16   userRequestBufferLen;
extern u8    userRequestBuffer[64];
/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
  {
    EP_NUM,
    1,
  };

DEVICE_PROP Device_Property =
  {
    USERHID_init,
    USERHID_Reset,
    USERHID_Status_In,
    USERHID_Status_Out,
    USERHID_Data_Setup,
    USERHID_NoData_Setup,
    USERHID_Get_Interface_Setting,
    USERHID_GetDeviceDescriptor,
    USERHID_GetConfigDescriptor,
    USERHID_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };
USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    USERHID_GetConfiguration,
    USERHID_SetConfiguration,
    USERHID_GetInterface,
    USERHID_SetInterface,
    USERHID_GetStatus,
    USERHID_ClearFeature,
    USERHID_SetEndPointFeature,
    USERHID_SetDeviceFeature,
    USERHID_SetDeviceAddress
  };

ONE_DESCRIPTOR Device_Descriptor =
  {
    (u8*)USERHID_DeviceDescriptor,
    USERHID_SIZ_DEVICE_DESC
  };

ONE_DESCRIPTOR Config_Descriptor =
  {
    (u8*)USERHID_ConfigDescriptor,
    USERHID_SIZ_CONFIG_DESC
  };

ONE_DESCRIPTOR INT1_Report_Descriptor =
  {
    (u8 *)INT1_ReportDescriptor,
    INT1_SIZ_REPORT_DESC
  };


ONE_DESCRIPTOR INT1_Hid_Descriptor =
  {
    (u8*)USERHID_ConfigDescriptor + INT1_OFF_HID_DESC,
    INT1_SIZ_HID_DESC
  };


ONE_DESCRIPTOR String_Descriptor[5] =
  {
    {(u8*)USERHID_StringLangID, USERHID_SIZ_STRING_LANGID},
    {(u8*)USERHID_StringVendor, USERHID_SIZ_STRING_VENDOR},
    {(u8*)USERHID_StringProduct, USERHID_SIZ_STRING_PRODUCT},
    {(u8*)USERHID_StringSerial, USERHID_SIZ_STRING_SERIAL},
    {((u8*)USERHID_StringVersion),USERHID_SIZ_STRING_VERSION},
  };

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : CustomHID_init.
* Description    : Custom HID init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USERHID_init(void)
{
  /* Update the serial number string descriptor with the data from the unique 
  ID*/
  
  Get_SerialNum();
  
  pInformation->Current_Configuration = 0;
  /* Connect the device */
  PowerOn();
  /* USB interrupts initialization */
  _SetISTR(0);               /* clear pending interrupts */
  wInterrupt_Mask = IMR_MSK;
  _SetCNTR(wInterrupt_Mask); /* set interrupts mask */
  pInformation->Current_Feature = USERHID_ConfigDescriptor[7];

  bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : CustomHID_Reset.
* Description    : Custom HID reset routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USERHID_Reset(void)
{
  /* Set Joystick_DEVICE as not configured */
  pInformation->Current_Configuration = 0;
  pInformation->Current_Interface = 0;/*the default Interface*/
  
  /* Current Feature initialization */
  pInformation->Current_Feature = USERHID_ConfigDescriptor[7];
  
  _SetBTABLE(BTABLE_ADDRESS);

  /* Initialize Endpoint 0 */
  _SetEPType(ENDP0, EP_CONTROL);
  _SetEPTxStatus(ENDP0, EP_TX_STALL);
  _SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  _SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  _Clear_Status_Out(ENDP0);
  _SetEPRxCount(ENDP0, 0x40);//Device_Property.MaxPacketSize);
  _SetEPRxStatus(ENDP0,EP_RX_VALID);

  /* Initialize Endpoint 1 */
  _SetEPType(ENDP1, EP_INTERRUPT);
  _SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  _SetEPRxAddr(ENDP1, ENDP1_RXADDR);
  _SetEPTxCount(ENDP1, 64);
  _SetEPRxCount(ENDP1, 64);
  _SetEPRxStatus(ENDP1, EP_RX_VALID);
  _SetEPTxStatus(ENDP1, EP_TX_NAK);
  
  bDeviceState = ATTACHED;
  
  /* Set this device to response on default address */
  SetDeviceAddress(0);
}

/*******************************************************************************
* Function Name  : CustomHID_SetConfiguration.
* Description    : Udpade the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USERHID_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}
/*******************************************************************************
* Function Name  : CustomHID_Status_In.
* Description    : Joystick status IN routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USERHID_Status_In(void)
{
  if(deviceModeBuffer[0] == 0x02){
    deviceModeBuffer[0] = 0;
    // Prepare to switch between the Bootloader and user application
    NVIC_SetSystemHandlerPendingBit(SystemHandler_PSV);
  }
}

/*******************************************************************************
* Function Name  : CustomHID_Status_Out
* Description    : Joystick status OUT routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USERHID_Status_Out (void)
{
}

u8* DeviceGetFeatureCopyRoutine(u16 Length)
{
  u32  wOffset;
  wOffset = pInformation->Ctrl_Info.Usb_wOffset;
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(DeviceFeature) - wOffset;
    return 0;
  }

  return deviceFeatureBuffer + wOffset;
}
u8* DeviceSetFeatureCopyRoutine(u16 Length)
{
  u32  wOffset;
  wOffset = pInformation->Ctrl_Info.Usb_wOffset;
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = 2;
    return (u8*)2;
  }
  else
  {
    return deviceModeBuffer + wOffset;
  }
}

/*******************************************************************************
* Function Name  : CustomHID_Data_Setup
* Description    : Handle the data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT USERHID_Data_Setup(u8 RequestNo)
{
  u8 *(*CopyRoutine)(u16);
  CopyRoutine = NULL;

  if ((RequestNo == GET_DESCRIPTOR)
      && (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
      )
  {
        if (pInformation->USBwValue1 == REPORT_DESCRIPTOR)
        {
          
             if( pInformation->USBwIndex0 == 0 )
              {
                CopyRoutine = INT1_GetReportDescriptor;
              }
//             else if( pInformation->USBwIndex0 == 1 )
//              {
//                CopyRoutine = INT2_GetReportDescriptor;
//              }
//             else if( pInformation->USBwIndex0 == 2 )
//              {
//                CopyRoutine = INT3_GetReportDescriptor;
//              }
        }
        else if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE)
        {          
              if( pInformation->USBwIndex0 == 0 )
              {
                CopyRoutine = INT1_GetHIDDescriptor;
              }
//              else if( pInformation->USBwIndex0 == 1 )
//              {
//                CopyRoutine = INT2_GetHIDDescriptor;
//              }
//              else if( pInformation->USBwIndex0 == 2 )
//              {
//                CopyRoutine = INT3_GetHIDDescriptor;
//              }
        }

  } /* End of GET_DESCRIPTOR */

  /*** GET_PROTOCOL ***/
  else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
          && RequestNo == GET_PROTOCOL)
  {
    CopyRoutine = USERHID_GetProtocolValue;
  }
  else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
          && RequestNo == GET_IDLE)
  {
    CopyRoutine = USERHID_GetIdleValue;
  }
  /*** SET_REPORT ***/
  else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
           && (RequestNo == SET_REPORT) )
  {
    // Set feature, Report ID is 2
    if(pInformation->USBwIndex0 == 0 
    && pInformation->USBwValue == MAKE_ID_TYPE(2,GET_REPORT_FEATURE)){
      CopyRoutine = DeviceSetFeatureCopyRoutine;
    }
  }
  /*** Get Report ***/
  else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
           && (RequestNo == GET_REPORT) )
  {
    // Get feature, Report ID is 1, Get the device Flash size, RAM size
    // And current work mode
    if(pInformation->USBwIndex0 == 0 
    && pInformation->USBwValue == MAKE_ID_TYPE(1,GET_REPORT_FEATURE)){
      CopyRoutine = DeviceGetFeatureCopyRoutine;
    }
  }
  /* User defined get request */
  else if(RequestNo == USER_GET){
    //userRequestBufferLen = 
      //pfnUserReq(pInformation->USBwValue,userRequestBuffer);
    CopyRoutine = UserCopyRoutine;
  }

  if (CopyRoutine == NULL)
  {
    return USB_UNSUPPORT;
  }

  pInformation->Ctrl_Info.CopyData = CopyRoutine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*CopyRoutine)(0);
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : CustomHID_NoData_Setup
* Description    : handle the no data class specific requests
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT USERHID_NoData_Setup(u8 RequestNo)
{
  if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
      && (RequestNo == SET_PROTOCOL))
  {
    return USERHID_SetProtocol();
  }
  else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
      && (RequestNo == SET_REPORT))
  {
    return USB_SUCCESS;
  }
  else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
      && (RequestNo == SET_IDLE))
  {
    return USERHID_SetIdle();
  }
  else
  {
    return USB_UNSUPPORT;
  }
}

/*******************************************************************************
* Function Name  : CustomHID_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
u8 *USERHID_GetDeviceDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : CustomHID_GetConfigDescriptor.
* Description    : Gets the configuration descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
u8 *USERHID_GetConfigDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : CustomHID_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
u8 *USERHID_GetStringDescriptor(u16 Length)
{
  u8 wValue0 = pInformation->USBwValue0;
  if (wValue0 > 4)
  {
    if(wValue0 == 0x1A){
      readPtr = writePtr;
      SetEPRxStatus(ENDP1, EP_RX_VALID);
    }
    return NULL;
  }
  else 
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
}

/*******************************************************************************
* Function Name  : CustomHID_GetReportDescriptor.
* Description    : Gets the HID report descriptor.
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
u8 *INT1_GetReportDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &INT1_Report_Descriptor);
}

/*******************************************************************************
* Function Name  : INT1_GetHIDDescriptor, INT2_GetHIDDescriptor, INT3_GetHIDDescriptor
* Description    : Gets the HID descriptor for each interface
* Input          : Length
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
u8 *INT1_GetHIDDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &INT1_Hid_Descriptor);
}

/*******************************************************************************
* Function Name  : CustomHID_Get_Interface_Setting.
* Description    : tests the interface and the alternate setting according to the
*                  supported one.
* Input          : - Interface : interface number.
*                  - AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : USB_SUCCESS or USB_UNSUPPORT.
*******************************************************************************/
RESULT USERHID_Get_Interface_Setting(u8 Interface, u8 AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 2)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : CustomHID_SetProtocol
* Description    : Joystick Set Protocol request routine.
* Input          : None.
* Output         : None.
* Return         : USB SUCCESS.
*******************************************************************************/
RESULT USERHID_SetProtocol(void)
{
  u8 wValue0 = pInformation->USBwValue0;
  idleAndProtocol.protocol[pInformation->USBwIndex0] = wValue0;
  return USB_SUCCESS;
}


/*******************************************************************************
* Function Name  : CustomHID_GetProtocolValue
* Description    : get the protocol value
* Input          : Length.
* Output         : None.
* Return         : address of the protcol value.
*******************************************************************************/
u8 *USERHID_GetProtocolValue(u16 Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = 1;
    return NULL;
  }
  else
  {
    return (u8 *)(&idleAndProtocol.protocol[pInformation->USBwIndex0]);
  }
}


/*******************************************************************************
* Function Name  : CustomHID_SetProtocol
* Description    : Joystick Set Idle request routine.
* Input          : None.
* Output         : None.
* Return         : USB SUCCESS.
*******************************************************************************/
RESULT USERHID_SetIdle(void)
{
  u8 wValue0 = pInformation->USBwValue1;
  /// High byte is the duration value
  idleAndProtocol.idle[pInformation->USBwIndex0] = wValue0;
  return USB_SUCCESS;
}


/*******************************************************************************
* Function Name  : CustomHID_GetProtocolValue
* Description    : get the idle value
* Input          : Length.
* Output         : None.
* Return         : address of the idle value.
*******************************************************************************/
u8 *USERHID_GetIdleValue(u16 Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = 1;
    return NULL;
  }
  else
  {
    return (u8 *)(&idleAndProtocol.idle[pInformation->USBwIndex0]);
  }
}


/*******************************************************************************
* Function Name  : CustomHID_SetConfiguration.
* Description    : Udpade the device state to configured and command the ADC 
*                  conversion.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/

void USERHID_GetConfiguration(void)
{
  //ECG_GetConfiguration();
}

void USERHID_SetConfiguration(void)
{
  if (pInformation->Current_Configuration != 0)
  {
    /* Device configured */
    idleAndProtocol.idle[0] = 0;
    idleAndProtocol.idle[1] = 0;
    idleAndProtocol.idle[2] = 0;
    idleAndProtocol.protocol[0] = 1;
    idleAndProtocol.protocol[1] = 1;
    idleAndProtocol.protocol[2] = 1;
    bDeviceState = CONFIGURED;
    readPtr = writePtr;
  }
}


////////////////////////////////////////////////////
/// User Get request
u16   userRequestBufferLen;
u8    userRequestBuffer[64];
u8* UserCopyRoutine(u16 Length)
{
  u32  wOffset;

  wOffset = pInformation->Ctrl_Info.Usb_wOffset;

  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = userRequestBufferLen - wOffset;
    return 0;
  }

  return userRequestBuffer + wOffset;
}

/*
 Because the zero is the default value for StatusInfo,
 That is BUS powered, and remote wake up disabled,
 So there is no need to override the GetStatus function for our device
 */
extern u16_u8 StatusInfo;
void USERHID_GetStatus(void)
{
  //StatusInfo.w = 0;
}
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
