
/*********************************INCLUDES******************************************/
#include "BSP/pcal6524.h"
#include "i2c.h"
#include <stdint.h>

/**********************GLOBAL VARIABLES AND CONSTANTS*******************************/


/****************************FORWARD DECLARATIONS***********************************/
static pcal6524_s pcal6524[3];


/*********************************FUNCTIONS*****************************************/
void PCAL6524_init(I2C_HandleTypeDef *i2c_handle, uint16_t i2c_addr_u1, uint16_t i2c_addr_u2, uint16_t i2c_addr_u5, uint32_t timeout_ms)
{
    uint16_t i2c_addr[3] = {i2c_addr_u1, i2c_addr_u2, i2c_addr_u5};
    for (int i = 0; i < 3; i++) {
        pcal6524[i].i2c_handle = i2c_handle;
        pcal6524[i].i2c_addr = (i2c_addr[i] << 1);
        pcal6524[i].timeout_ms = timeout_ms;
        pcal6524[i].init_complete = 1;
    }
    
}

uint32_t Read_PCAL_DeviceID(uint8_t i2c_address)
{
    
    

    
    // The NXP "Reserved Device ID" 7-bit address is 1111 100 (0x7C)
    // Write byte: 1111 1000 (0xF8), Read byte: 1111 1001 (0xF9)
    const uint16_t RESERVED_ADDR = (0x7C << 1); 
    
    // The target chip address is sent as the "register" byte in this special protocol
    uint16_t target_chip_addr_byte = (i2c_address << 1);
    
    uint8_t buffer[3] = {0};
    HAL_StatusTypeDef status;

    

    /**
      * 2. EXECUTE NXP DEVICE ID PROTOCOL:
      * This Mem_Read call executes exactly what the datasheet Section 6.3.2 requires:
      * - START
      * - Send 0xF8 (RESERVED_ADDR Write)
      * - Send target_chip_addr_byte (Target chip to identify)
      * - Re-START (Crucial: STOP would reset the state machine)
      * - Send 0xF9 (RESERVED_ADDR Read)
      * - Read 3 Bytes (Manufacturer, Part, Revision)
      * - STOP (Automatically NACKs the last byte per HAL implementation)
      */
    status = HAL_I2C_Mem_Read(&hi2c1, 
                              RESERVED_ADDR, 
                              target_chip_addr_byte, 
                              I2C_MEMADD_SIZE_8BIT, 
                              buffer, 3, 100);

    if (status == HAL_OK)
    {
        // Result Breakdown per NXP:
        // Byte 0: 8 bits of Manufacturer ID
        // Byte 1: 4 bits Manuf ID + 4 bits Part ID
        // Byte 2: 5 bits Part ID + 3 bits Revision
        return ((uint32_t)buffer[0] << 16) | ((uint32_t)buffer[1] << 8) | (uint32_t)buffer[2];
    }

    // Failure: Return the HAL status encoded
    return (uint32_t)(0xEEEE0000 | status); 
}

// ===================================================================
// PCAL6524 INITIALIZATION & CONTROL
// ===================================================================

/**
  * @brief  Configures a PCAL6524 chip for 24-bit input with pull-ups and 5ms debounce
  * using an external 32kHz clock provided on IO0_0.
  * @param  i2c_address: 7-bit address (0x20, 0x21, or 0x22)
  */
HAL_StatusTypeDef PCAL6524_register_init(void)
{
    uint8_t all_ones[3] = {0xFF, 0xFF, 0xFF};
    uint8_t all_zeros[3] = {0x00, 0x00, 0x00};
    
    // Debounce enable for P0 and P1 only — Port 2 has no debounce hardware.
    // P0_0 excluded (0xFE) because it carries the 32kHz external clock input.
    uint8_t debounce_en[2] = {0xFE, 0xFF};

    // Debounce Count: t_DP = T_clk × count → 5ms at 32kHz: (1/32000) × count = 0.005 → count = 160
    uint8_t debounce_val = 160;

    HAL_StatusTypeDef status;
    for (int i = 0; i < 3; i++) {
        if (pcal6524[i].init_complete) {
            // 1. Configuration: Set all 24 pins as INPUTS
            status = HAL_I2C_Mem_Write(pcal6524[i].i2c_handle, pcal6524[i].i2c_addr, REG_CONFIG_P0, I2C_MEMADD_SIZE_8BIT, all_ones, 3, pcal6524[i].timeout_ms);
            if (status != HAL_OK) return status;

            // 2. Pull-up/down Enable: Enable internal resistors
            status = HAL_I2C_Mem_Write(pcal6524[i].i2c_handle, pcal6524[i].i2c_addr, REG_PUD_EN_P0, I2C_MEMADD_SIZE_8BIT, all_ones, 3, pcal6524[i].timeout_ms);
            if (status != HAL_OK) return status;

            // 3. Pull-up/down Selection: Select PULL-UP (Active Low Buttons)
            status = HAL_I2C_Mem_Write(pcal6524[i].i2c_handle, pcal6524[i].i2c_addr, REG_PUD_SEL_P0, I2C_MEMADD_SIZE_8BIT, all_ones, 3, pcal6524[i].timeout_ms);
            if (status != HAL_OK) return status;

            // 4. Switch Debounce Enable: P0 and P1 only (Port 2 has no debounce hardware)
            status = HAL_I2C_Mem_Write(pcal6524[i].i2c_handle, pcal6524[i].i2c_addr, REG_SW_DEBOUNCE_EN_P0, I2C_MEMADD_SIZE_8BIT, debounce_en, 2, pcal6524[i].timeout_ms);
            if (status != HAL_OK) return status;

            // 5. Switch Debounce Count: ~5ms at 32kHz external clock
            status = HAL_I2C_Mem_Write(pcal6524[i].i2c_handle, pcal6524[i].i2c_addr, REG_SW_DEBOUNCE_COUNT, I2C_MEMADD_SIZE_8BIT, &debounce_val, 1, pcal6524[i].timeout_ms);
            if (status != HAL_OK) return status;

            // 6. Wait 9 clock cycles at 32kHz (~281us) for debounce clock to settle (datasheet Fig. 13)
            HAL_Delay(1);

            // 7. Interrupt Mask: Unmask all interrupts (0=Enable)
            status = HAL_I2C_Mem_Write(pcal6524[i].i2c_handle, pcal6524[i].i2c_addr, REG_INT_MASK_P0, I2C_MEMADD_SIZE_8BIT, all_zeros, 3, pcal6524[i].timeout_ms);
        }
    }
    

    return status;
}