/*********************************INCLUDES******************************************/
#include "BSP/ws2812b.h"
#include "stm32f1xx_hal_tim.h"
#include <string.h>

/**********************GLOBAL VARIABLES AND CONSTANTS*******************************/
ws2812b_s ws2812b;
led_color_t led_buffer[NUM_OF_LEDS];
volatile uint8_t g_flag_led_update = 0;
volatile uint8_t g_simhub_connected = 0;

// DMA buffer: 24 bits per LED + reset slots (zeros for >50µs low)
static volatile uint16_t pwm_data[NUM_OF_LEDS * NUM_OF_BITS + RESET_SLOTS];

// Flag to track if DMA transfer is in progress
static volatile uint8_t dma_busy = 0;

/*********************************FUNCTIONS*****************************************/

/**
 * @brief  Initialize WS2812B driver with timer and channel
 * @param  htim: pointer to timer handle (e.g. &htim1)
 * @param  channel: timer channel (e.g. TIM_CHANNEL_1)
 */
void ws2812_init(TIM_HandleTypeDef *htim, uint32_t channel)
{
    ws2812b.tim_handle = htim;
    ws2812b.tim_channel = channel;
    ws2812b.init_complete = 1;

    // Clear LED buffer to all off
    memset(led_buffer, 0, sizeof(led_buffer));
    memset((void *)pwm_data, 0, sizeof(pwm_data));
}

/**
 * @brief  Set a single LED color in the buffer (does NOT send to LEDs)
 * @param  index: LED index (0 to NUM_OF_LEDS-1)
 * @param  r: red value (0-255)
 * @param  g: green value (0-255)
 * @param  b: blue value (0-255)
 */
void ws2812_set_led(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if (index >= NUM_OF_LEDS) return;
    led_buffer[index].r = r;
    led_buffer[index].g = g;
    led_buffer[index].b = b;
}

/**
 * @brief  Set all LEDs to the same color in the buffer (does NOT send)
 * @param  r: red value (0-255)
 * @param  g: green value (0-255)
 * @param  b: blue value (0-255)
 */
void ws2812_set_all(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < NUM_OF_LEDS; i++) {
        led_buffer[i].r = r;
        led_buffer[i].g = g;
        led_buffer[i].b = b;
    }
}

/**
 * @brief  Clear all LEDs (set to black) in the buffer (does NOT send)
 */
void ws2812_clear(void)
{
    memset(led_buffer, 0, sizeof(led_buffer));
}

/**
 * @brief  Send the entire LED buffer to the WS2812B strip via DMA
 *         Converts led_buffer[] RGB to WS2812B GRB bit stream
 */
void ws2812_update(void)
{
    if (!ws2812b.init_complete) return;
    if (dma_busy) return; // Don't start a new transfer while one is running

    uint16_t pos = 0;

    for (int led = 0; led < NUM_OF_LEDS; led++) {
        // WS2812B expects GRB order
        uint32_t color = ((uint32_t)led_buffer[led].g << 16) |
                         ((uint32_t)led_buffer[led].r << 8)  |
                          (uint32_t)led_buffer[led].b;

        for (int bit = 23; bit >= 0; bit--) {
            pwm_data[pos++] = (color & (1 << bit)) ? WS2812_HI : WS2812_LO;
        }
    }

    // Reset period: all zeros to keep data line low for >50µs
    for (int i = 0; i < RESET_SLOTS; i++) {
        pwm_data[pos++] = 0;
    }

    dma_busy = 1;
    HAL_TIM_PWM_Start_DMA(ws2812b.tim_handle, ws2812b.tim_channel,
                          (uint32_t *)pwm_data, pos);
}

/**
 * @brief  DMA transfer complete callback — stop PWM output
 *         Add this call inside HAL_TIM_PWM_PulseFinishedCallback()
 *         in your interrupt handler (stm32f1xx_it.c or main.c)
 */
void ws2812_dma_complete_callback(TIM_HandleTypeDef *htim)
{
    if (htim == ws2812b.tim_handle) {
        HAL_TIM_PWM_Stop_DMA(ws2812b.tim_handle, ws2812b.tim_channel);
        dma_busy = 0;
    }
}