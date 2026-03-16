
/*********************************INCLUDES******************************************/
#include "BSP/ws2812b.h"
#include "stm32f1xx_hal_tim.h"


/**********************GLOBAL VARIABLES AND CONSTANTS*******************************/
ws2812b_s ws2812b; // Structure to hold WS2812B configuration
volatile uint16_t pwm_data[NUM_OF_LEDS * NUM_OF_BITS]; // 24 bits per LED, 8 bits per color (R, G, B)

/****************************FORWARD DECLARATIONS***********************************/



/*********************************FUNCTIONS*****************************************/
void ws2812_init(TIM_HandleTypeDef *htim, uint32_t channel)
{
    ws2812b.tim_handle = htim; // Assign the timer handle to the ws2812b structure
    ws2812b.tim_channel = channel; // Assign the timer channel to the ws2812b structure
    ws2812b.init_complete = 1; // Mark the initialization as complete
}

void ws2812_send(uint8_t green, uint8_t red, uint8_t blue)
{
    uint32_t data = (green << 16) | (red << 8) | blue; // Combine RGB values into a single 24-bit value
    uint8_t j = 0;
    // Fill the pwm_data array with the color values for each LED
    for (int i = 0; i < NUM_OF_LEDS * NUM_OF_BITS - 1; i++) {
        if (j >= 24) {
            j = 0; // Reset j after processing 24 bits
        }
        if (data & (1 << (23 - j))) {
            pwm_data[i] = 60; // Set high bit
        } else {
            pwm_data[i] = 30; // Set low bit
        }
        j++;
    }

    // Here you would typically start the timer to send the data via PWM
    // This part is hardware-specific and depends on your setup.
    HAL_TIM_PWM_Start_DMA(ws2812b.tim_handle, ws2812b.tim_channel, (uint32_t *)pwm_data, sizeof(pwm_data));
}
