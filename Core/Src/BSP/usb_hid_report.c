
/*********************************INCLUDES******************************************/
#include "BSP/usb_hid_report.h"
#include "BSP/adc_input.h"
#include "stm32f1xx_hal.h"
#include "usbd_custom_hid_if.h"
#include "usb_device.h"

/**********************GLOBAL VARIABLES AND CONSTANTS*******************************/
HID_InputReport_t HID_InputReport;


/****************************FORWARD DECLARATIONS***********************************/


/*********************************FUNCTIONS*****************************************/
void usb_hid_init(void)
{
    HID_InputReport.reportID = 1; // Set Report ID to 1
    HID_InputReport.padding = 0; // Padding byte for alignment
    for (int i = 0; i < 8; i++) 
    {
        HID_InputReport.buttons[i] = 0; // Initialize buttons to 0 
    }
    for (int i = 0; i < 8; i++) 
    {
        HID_InputReport.axis[i] = 0; // Initialize axes to 0
    }
}

void usb_hid_send_report(HID_InputReport_t *report)
{
    for (int i = 0; i < 6; i++) 
    {
        HID_InputReport.axis[i] = adc_update_input[i]; // Initialize axes to 0
    }
    // Send the HID report over USB
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)report, sizeof(HID_InputReport_t));
}
