

/*********************************INCLUDES******************************************/
#include "BSP/helpers.h"
#include "BSP/usb_hid_report.h"
#include "stm32f1xx_it.h"
#include <stdint.h>

/**********************GLOBAL VARIABLES AND CONSTANTS*******************************/

/****************************FORWARD DECLARATIONS***********************************/


/*********************************FUNCTIONS*****************************************/
void assign_inputs_to_HID_structure(uint8_t inputs[3], uint8_t offset)
{
  
  for (int i = 0; i < 2; i++) {
      if (i == 0) {
        inputs[i] |= ~0xFE;
      }
      // Invert because buttons are active-low (pressed=0, released=1)
      HID_InputReport.buttons[offset + i] = ~inputs[i];
  }
}
