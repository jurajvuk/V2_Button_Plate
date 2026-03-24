#ifndef __WS2812B_H
#define __WS2812B_H

/*********************************INCLUDES******************************************/
#include "stm32f1xx_hal.h"

/***********************************DEFINES*****************************************/
#define NUM_OF_LEDS       52
#define NUM_OF_BITS       24
#define RESET_SLOTS       50    // ~50µs reset period (zeros at end of DMA buffer)

// WS2812B timing (for 72MHz timer with ARR=89 → 1.25µs per bit)
#define WS2812_HI         60    // ~0.83µs high for '1' bit
#define WS2812_LO         30    // ~0.42µs high for '0' bit

// LED map — physical chain order on PCB
#define LED_BUTTONS_START       0
#define LED_BUTTONS_COUNT       20    // 10 buttons × 2 LEDs

#define LED_FUNKY_START         20
#define LED_FUNKY_COUNT         8     // 2 funky switches × 4 LEDs

#define LED_ENCODER_START       28
#define LED_ENCODER_COUNT       8     // 2 encoders × 4 LEDs

#define LED_RPM_START           36
#define LED_RPM_COUNT           10    // 10 RPM indicator LEDs

#define LED_FLAGS_START         46
#define LED_FLAGS_COUNT         6     // 3 left + 3 right flag LEDs

/***********************************TYPEDEFS****************************************/
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} led_color_t;

typedef struct {
    TIM_HandleTypeDef *tim_handle;
    uint32_t tim_channel;
    uint8_t init_complete;
} ws2812b_s;

/****************************GLOBAL VARIABLES***************************************/
extern ws2812b_s ws2812b;
extern led_color_t led_buffer[NUM_OF_LEDS];
extern volatile uint8_t g_flag_led_update;
extern volatile uint8_t g_simhub_connected;

/****************************FUNCTION PROTOTYPES************************************/
void ws2812_init(TIM_HandleTypeDef *htim, uint32_t channel);
void ws2812_set_led(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void ws2812_set_all(uint8_t r, uint8_t g, uint8_t b);
void ws2812_clear(void);
void ws2812_update(void);
void ws2812_dma_complete_callback(TIM_HandleTypeDef *htim);

#endif /* __WS2812B_H */