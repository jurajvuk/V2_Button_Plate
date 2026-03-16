#ifndef SHIFTERS_H_
#define SHIFTERS_H_

/*********************************INCLUDES******************************************/
#include "stm32f1xx_hal.h"

/**********************************DEFINES******************************************/
#define SHIFTER_LIMIT 400
#define SHIFTER_ATTACK_POINT 0.8f
#define SHIFTER_INDEX 0

/*********************************STRUCTURES****************************************/


/******************************EXTERN VARIABLES*************************************/

extern volatile uint16_t shifter_attack_point[8];
extern volatile int16_t shifter_magnet_orientation[8];

/****************************FUNCTION PROTOTYPES************************************/
void shifter_init(void);
void shifter_update(void);

#endif /* SHIFTERS_H_ */
