#ifndef WS2812B_H_
#define WS2812B_H_

/*********************************INCLUDES******************************************/
#include "stm32f1xx_hal.h"

/**********************************DEFINES******************************************/
#define NUM_OF_LEDS 52
#define NUM_OF_BITS 24 // 24 bits per LED (8 bits for each color: R, G, B)


/*********************************STRUCTURES****************************************/
typedef struct {
	TIM_HandleTypeDef *tim_handle;
	uint32_t tim_channel;
	uint8_t init_complete;
}ws2812b_s;

/******************************EXTERN VARIABLES*************************************/
extern volatile uint16_t pwm_data[NUM_OF_LEDS * NUM_OF_BITS]; // 24 bits per LED, 8 bits per color (R, G, B)


/****************************FUNCTION PROTOTYPES************************************/
void ws2812_init(TIM_HandleTypeDef *htim, uint32_t channel);
void ws2812_send(uint8_t green, uint8_t red, uint8_t blue);

#endif /* WS2812B_H_ */
