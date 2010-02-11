// F:\Codes\oled\STM32\oldeLoader\oledLoader1.h


char ReportDescriptor[49] = {
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

