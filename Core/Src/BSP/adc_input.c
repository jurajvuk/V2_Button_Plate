

/*********************************INCLUDES******************************************/
#include "BSP/adc_input.h"

/**********************GLOBAL VARIABLES AND CONSTANTS*******************************/
volatile uint16_t adc_raw_input[6] = {0};
volatile uint16_t adc_base_input[6] = {0};
volatile uint16_t adc_update_input[6] = {0};
/****************************FORWARD DECLARATIONS***********************************/


/*********************************FUNCTIONS*****************************************/
void adc_input_init(void)
{
    // Initialize ADC input array to zero
    for (int i = 0; i < 6; i++) {
        adc_base_input[i] = adc_raw_input[i];
    }
}

void adc_input_update(void)
{
    // Update the adc_update_input
    for (int i = 0; i < 6; i++) {
        adc_update_input[i] = adc_raw_input[i];
    }
}
