#ifndef PCAL6524_H_
#define PCAL6524_H_

/*********************************INCLUDES******************************************/
#include "stm32f1xx_hal.h"

/**********************************DEFINES******************************************/
#define FIRST_PCAL6524_I2C_ADDR   	0x20
#define SECOND_PCAL6524_I2C_ADDR   	0x21
#define THIRD_PCAL6524_I2C_ADDR   	0x22

// ===================================================================
// PCAL6524 REGISTER MAP (Corrected for Agile I/O & External Debounce)
// ===================================================================
#define REG_INPUT_P0            0x00  // Input Port 0
#define REG_OUTPUT_P0           0x04  // Output Port 0
#define REG_CONFIG_P0           0x0C  // Configuration Port 0 (0=Output, 1=Input)
#define REG_INT_MASK_P0         0x54  // Interrupt Mask Port 0 (0=Enable, 1=Disable)
#define REG_PUD_EN_P0           0x4C  // Pull-up/down Enable Port 0
#define REG_PUD_SEL_P0          0x50  // Pull-up/down Selection Port 0 (0=PD, 1=PU)

// Debounce Registers
#define REG_DEBOUNCE_EN_P0      0x48  // Debounce Enable Port 0 (0=Disable, 1=Enable)
#define REG_DEBOUNCE_COUNT      0x4B  // Debounce Count (Global for all ports)

/*********************************STRUCTURES****************************************/
typedef struct {
	I2C_HandleTypeDef *i2c_handle;
	uint16_t i2c_addr_first;
    uint16_t i2c_addr_second;
    uint16_t i2c_addr_third;
	uint32_t timeout_ms;
	uint8_t init_complete;
}pcal6524_s;

/******************************EXTERN VARIABLES*************************************/



/****************************FUNCTION PROTOTYPES************************************/
void pcal_init(I2C_HandleTypeDef *ui2c_handle, uint16_t i2c_addr_first, uint16_t i2c_addr_second, uint16_t i2c_addr_third, uint32_t timeout_ms);
uint32_t Read_PCAL_DeviceID(uint8_t i2c_address);
HAL_StatusTypeDef PCAL6524_Init(uint8_t i2c_address);

#endif /* PCAL6524_H_ */
