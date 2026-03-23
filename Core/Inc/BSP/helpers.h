#ifndef HELPERS_H_
#define HELPERS_H_

/*********************************INCLUDES******************************************/
#include "stm32f1xx_hal.h"
#include <stdint.h>

/**********************************DEFINES******************************************/


/*********************************STRUCTURES****************************************/


/******************************EXTERN VARIABLES*************************************/

/****************************FUNCTION PROTOTYPES************************************/
void assign_inputs_to_HID_structure(uint8_t inputs[3], uint8_t offset);


#endif /* HELPERS_H_ */
