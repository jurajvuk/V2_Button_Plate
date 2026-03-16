
/*********************************INCLUDES******************************************/
#include "BSP/calibration.h"
#include "stm32f1xx_hal.h"
#include "usbd_custom_hid_if.h"
#include "usb_device.h"
#include "main.h"
#include "BSP/adc_input.h"

/**********************GLOBAL VARIABLES AND CONSTANTS*******************************/
volatile HID_CalibrationReport_t g_received_report_id_4;
volatile HID_CalibrationReport_t g_latestCalibrationReport;
/****************************FORWARD DECLARATIONS***********************************/


/*********************************FUNCTIONS*****************************************/
void calibration_init(void) {
    // Initialize the calibration report structure
    g_received_report_id_4.reportID = 0; // Set the report ID for calibration
    g_received_report_id_4.calibrationStart = 0; // Default to not started
    g_received_report_id_4.axisIndex = 0; // Start with the first axis
    g_received_report_id_4.padding = 0; // Padding byte for alignment
    g_latestCalibrationReport.reportID = 0; // Initialize latest calibration report ID
    g_latestCalibrationReport.calibrationStart = 0; // Default to not started
    g_latestCalibrationReport.axisIndex = 0; // Start with the first axis
    g_latestCalibrationReport.padding = 0; // Padding byte for alignment
    
    for (int i = 0; i < 12; i++) {
        g_received_report_id_4.minMaxCalibrationValues[i] = 0; // Initialize minMaxCalibrationValues to 0
        g_latestCalibrationReport.minMaxCalibrationValues[i] = 0; // Initialize minMaxCalibrationValues to 0
    }/*
    for (int i = 0; i < 6; i++) {
        g_received_report_id_4.minMaxCalibrationValues[i * 2] = adc_base_input[i]; 
        //g_latestCalibrationReport.minMaxCalibrationValues[i * 2] = adc_base_input[i]; 
    }*/
}

void calibration_update(void)
{
    // Check if the calibration process has started
    if (g_received_report_id_4.calibrationStart == 0x01) 
    {
        if (g_received_report_id_4.axisIndex == 0x01)
        {
            //g_latestCalibrationReport.minMaxCalibrationValues[0] = g_received_report_id_4.minMaxCalibrationValues[0];
            g_latestCalibrationReport.minMaxCalibrationValues[1] = g_received_report_id_4.minMaxCalibrationValues[1];
        }
        else if (g_received_report_id_4.axisIndex == 0x02)
        {
            //g_latestCalibrationReport.minMaxCalibrationValues[2] = g_received_report_id_4.minMaxCalibrationValues[2];
            g_latestCalibrationReport.minMaxCalibrationValues[3] = g_received_report_id_4.minMaxCalibrationValues[3];
        }
        else if (g_received_report_id_4.axisIndex == 0x03)
        {
            //g_latestCalibrationReport.minMaxCalibrationValues[4] = g_received_report_id_4.minMaxCalibrationValues[4];
            g_latestCalibrationReport.minMaxCalibrationValues[5] = g_received_report_id_4.minMaxCalibrationValues[5];
        }
        else if (g_received_report_id_4.axisIndex == 0x04)
        {
            //g_latestCalibrationReport.minMaxCalibrationValues[6] = g_received_report_id_4.minMaxCalibrationValues[6];
            g_latestCalibrationReport.minMaxCalibrationValues[7] = g_received_report_id_4.minMaxCalibrationValues[7];
        }
        else if (g_received_report_id_4.axisIndex == 0x05)
        {
            Save_Calibration_Data(); // Save the calibration data to flash
        }
    }
}

/**
  * @brief  Saves the current calibration data from the global variable to flash memory.
  */
void Save_Calibration_Data(void)
{
    // 1. Unlock the Flash Memory for writing
    HAL_FLASH_Unlock();

    // 2. Erase the designated flash page
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;

    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = CALIBRATION_DATA_FLASH_ADDRESS;
    EraseInitStruct.NbPages     = 1; // We are only using one page

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
        // Error occurred during page erase.
        // You can add error handling here, like blinking an LED.
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        HAL_FLASH_Lock(); // Lock the flash on error
        return;
    }

    // 3. Program the new data to the flash page
    // We will write the 12 min/max values (24 bytes total)
    // The flash must be programmed in half-words (16 bits)
    for (int i = 0; i < 12; i++)
    {
        uint32_t address = CALIBRATION_DATA_FLASH_ADDRESS + (i * 2); // Each value is 2 bytes
        uint16_t data = g_latestCalibrationReport.minMaxCalibrationValues[i];

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, data) != HAL_OK)
        {
            // Error occurred during programming.
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
            HAL_FLASH_Lock(); // Lock the flash on error
            return;
        }
    }

    // 4. Lock the Flash Memory
    HAL_FLASH_Lock();
}

/**
  * @brief  Loads the calibration data from flash memory into the global variable.
  */
void Load_Calibration_Data(void)
{
    // Reading from flash is simple. We can treat it like a regular memory read.
    for (int i = 0; i < 12; i++)
    {
        uint32_t address = CALIBRATION_DATA_FLASH_ADDRESS + (i * 2);
        // Cast the memory address to a 16-bit pointer and read its value
        g_latestCalibrationReport.minMaxCalibrationValues[i] = *(volatile uint16_t*)address;
    }

    // Optional: Add a check to see if the loaded data is valid.
    // For example, if the first value is 0xFFFF (the erased state),
    // it means no data has been saved yet, so you might load default values.
    if (g_latestCalibrationReport.minMaxCalibrationValues[0] == 0xFFFF)
    {
        // This is the first run, load default values if needed
        for (int i = 0; i < 12; i++) 
        {
            g_latestCalibrationReport.minMaxCalibrationValues[i] = 0; // Initialize all values to 0
        }
    }
}
