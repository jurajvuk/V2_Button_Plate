
/*********************************INCLUDES******************************************/
#include "BSP/pcal6524.h"
#include "i2c.h"

/**********************GLOBAL VARIABLES AND CONSTANTS*******************************/


/****************************FORWARD DECLARATIONS***********************************/
static pcal6524_s pcal6524;


/*********************************FUNCTIONS*****************************************/
void pcal_init(I2C_HandleTypeDef *i2c_handle, uint16_t i2c_addr_first, uint16_t i2c_addr_second, uint16_t i2c_addr_third, uint32_t timeout_ms)
{
    pcal6524.i2c_handle = i2c_handle;
    pcal6524.i2c_addr_first = i2c_addr_first;
    pcal6524.i2c_addr_second = i2c_addr_second;
    pcal6524.i2c_addr_third = i2c_addr_third;
    pcal6524.timeout_ms = timeout_ms;
    pcal6524.init_complete = 1;
}

uint32_t Read_PCAL_DeviceID(uint8_t i2c_address)
{
    /*
    uint8_t device_id_cmd = 0x7F;
    uint8_t received_id_bytes[3] = {0};
    uint32_t device_id = 0;
    HAL_StatusTypeDef status;

    // The HAL library expects the 7-bit address shifted left by one
    uint16_t hal_address = (uint16_t)(i2c_address << 1);
    if (pcal6524.init_complete == 1)
    {
        // 1. Transmit the "read device ID" command byte
        status = HAL_I2C_Master_Transmit(pcal6524.i2c_handle, hal_address, &device_id_cmd, 1, pcal6524.timeout_ms); // 100ms timeout

        // 2. If the command was sent successfully, perform a read
        if (status == HAL_OK)
        {
            status = HAL_I2C_Master_Receive(pcal6524.i2c_handle, hal_address, received_id_bytes, 3, pcal6524.timeout_ms);

            if (status == HAL_OK)
            {
                // 3. If the read was successful, combine the bytes
                device_id = (uint32_t)(received_id_bytes[0] << 16) | 
                            (uint32_t)(received_id_bytes[1] << 8)  | 
                            (uint32_t)(received_id_bytes[2]);
            }
        }
    }
    

    return device_id;
    */
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
HAL_StatusTypeDef PCAL6524_Init(uint8_t i2c_address)
{
    uint16_t dev_addr = (i2c_address << 1);
    uint8_t all_ones[3] = {0xFF, 0xFF, 0xFF};
    uint8_t all_zeros[3] = {0x00, 0x00, 0x00};
    
    // Mask for Port 0 Debounce: 0xFE (1111 1110)
    // We disable debouncing on Pin 0 because it is the 32kHz clock input.
    uint8_t debounce_en[3] = {0xFE, 0xFF, 0xFF}; 
    
    // Debounce Count Calculation for 5ms with 32kHz external clock:
    // 5000us / (2 * (1/32000)) = 5000 / 62.5 = 80
    uint8_t debounce_val = 80; 

    HAL_StatusTypeDef status;

    // 1. Configuration: Set all 24 pins as INPUTS
    status = HAL_I2C_Mem_Write(&hi2c1, dev_addr, REG_CONFIG_P0, I2C_MEMADD_SIZE_8BIT, all_ones, 3, 100);
    if (status != HAL_OK) return status;

    // 2. Pull-up/down Enable: Enable internal resistors
    status = HAL_I2C_Mem_Write(&hi2c1, dev_addr, REG_PUD_EN_P0, I2C_MEMADD_SIZE_8BIT, all_ones, 3, 100);
    if (status != HAL_OK) return status;

    // 3. Pull-up/down Selection: Select PULL-UP (Active Low Buttons)
    status = HAL_I2C_Mem_Write(&hi2c1, dev_addr, REG_PUD_SEL_P0, I2C_MEMADD_SIZE_8BIT, all_ones, 3, 100);
    if (status != HAL_OK) return status;

    // 4. Debounce Enable: Enable for all pins except the clock input (IO0_0)
    status = HAL_I2C_Mem_Write(&hi2c1, dev_addr, REG_DEBOUNCE_EN_P0, I2C_MEMADD_SIZE_8BIT, debounce_en, 3, 100);
    if (status != HAL_OK) return status;

    // 5. Debounce Count: Set global time to ~5ms based on 32kHz source
    status = HAL_I2C_Mem_Write(&hi2c1, dev_addr, REG_DEBOUNCE_COUNT, I2C_MEMADD_SIZE_8BIT, &debounce_val, 1, 100);
    if (status != HAL_OK) return status;

    // 6. Interrupt Mask: Unmask all interrupts (0=Enable)
    status = HAL_I2C_Mem_Write(&hi2c1, dev_addr, REG_INT_MASK_P0, I2C_MEMADD_SIZE_8BIT, all_zeros, 3, 100);
    
    return status;
}