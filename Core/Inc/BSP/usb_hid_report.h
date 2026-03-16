#ifndef USB_HID_REPORT_H_
#define USB_HID_REPORT_H_

/*********************************INCLUDES******************************************/
#include "stm32f1xx_hal.h"
#include <stdint.h>

/**********************************DEFINES******************************************/


/*********************************STRUCTURES****************************************/
// Ensure no padding
#if defined(__GNUC__)
  #define PACKED   __attribute__((packed))
#else
  #define PACKED
  #pragma pack(push,1)
#endif

// — Report ID 1: 40 buttons + 6×12-bit axes (14 bytes payload + 1 ID)
typedef struct PACKED {
    uint8_t  reportID;       // == 1
    uint8_t  buttons[8];     // 64 bits = 8 bytes, LSB = Button1
    uint8_t padding;        // Padding byte for alignment
    // 8 axes, each 12-bit little-endian; we store in 16-bit slots:
    uint16_t axis[8];        // only lower 12 bits of each are valid
} HID_InputReport_t;         // sizeof = 1 + 8 + (8×2) = 25 bytes

/******************************EXTERN VARIABLES*************************************/
extern HID_InputReport_t HID_InputReport;


/****************************FUNCTION PROTOTYPES************************************/
void usb_hid_init(void);
void usb_hid_send_report(HID_InputReport_t *report);

#endif /* USB_HID_REPORT_H_ */
