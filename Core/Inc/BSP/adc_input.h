#ifndef ADC_INPUT_H_
#define ADC_INPUT_H_

/*********************************INCLUDES******************************************/
#include "stm32f1xx_hal.h"

/**********************************DEFINES******************************************/


/*********************************STRUCTURES****************************************/


/******************************EXTERN VARIABLES*************************************/
extern volatile uint16_t adc_raw_input[6];
extern volatile uint16_t adc_base_input[6];
extern volatile uint16_t adc_update_input[6];
/****************************FUNCTION PROTOTYPES************************************/
void adc_input_init(void);
void adc_input_update(void);

#endif /* ADC_INPUT_H_ */
