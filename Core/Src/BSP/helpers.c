

/*********************************INCLUDES******************************************/
#include "BSP/helpers.h"
#include "BSP/usb_hid_report.h"

/**********************GLOBAL VARIABLES AND CONSTANTS*******************************/

/****************************FORWARD DECLARATIONS***********************************/


/*********************************FUNCTIONS*****************************************/
void assign_inputs_to_HID_structure(uint8_t inputs[3])
{
    for (int i = 0; i < 3; i++) {
        if (i == 0) {
          inputs[i] |= ~0xFE;
        }
        HID_InputReport.buttons[i] = ~inputs[i];
      }
}
