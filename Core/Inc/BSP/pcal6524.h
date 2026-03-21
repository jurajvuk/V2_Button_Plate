#ifndef PCAL6524_H_
#define PCAL6524_H_

/*********************************INCLUDES******************************************/
#include "stm32f1xx_hal.h"

/**********************************DEFINES******************************************/
#define PCAL6524_I2C_ADDR_U1		0x20
#define PCAL6524_I2C_ADDR_U2	   	0x22
#define PCAL6524_I2C_ADDR_U5	   	0x21

// ===================================================================
// PCAL6524 REGISTER MAP (Corrected for Agile I/O & External Debounce)
// ===================================================================
#define REG_INPUT_P0            0x00  // Input Port 0
#define REG_OUTPUT_P0           0x04  // Output Port 0
#define REG_CONFIG_P0           0x0C  // Configuration Port 0 (0=Output, 1=Input)
#define REG_INT_MASK_P0         0x54  // Interrupt Mask Port 0 (0=Enable, 1=Disable)
#define REG_PUD_EN_P0           0x4C  // Pull-up/down Enable Port 0
#define REG_PUD_SEL_P0          0x50  // Pull-up/down Selection Port 0 (0=PD, 1=PU)

// Switch Debounce Registers (Agile I/O — P0 and P1 only; P2 has no debounce hardware)
#define REG_SW_DEBOUNCE_EN_P0   0x74  // Switch Debounce Enable Port 0 (1=Enable)
#define REG_SW_DEBOUNCE_EN_P1   0x75  // Switch Debounce Enable Port 1 (1=Enable)
#define REG_SW_DEBOUNCE_COUNT   0x76  // Switch Debounce Count (global, t_DP = T_clk × count)

/*********************************STRUCTURES****************************************/
typedef struct {
	I2C_HandleTypeDef *i2c_handle;
	uint16_t i2c_addr;
    uint32_t timeout_ms;
	uint8_t init_complete;
}pcal6524_s;

/******************************EXTERN VARIABLES*************************************/



/****************************FUNCTION PROTOTYPES************************************/
void PCAL6524_init(I2C_HandleTypeDef *ui2c_handle, uint16_t i2c_addr_u1, uint16_t i2c_addr_u2, uint16_t i2c_addr_u5, uint32_t timeout_ms);
uint32_t Read_PCAL_DeviceID(uint8_t i2c_address);
HAL_StatusTypeDef PCAL6524_register_init(void);

#endif /* PCAL6524_H_ */
