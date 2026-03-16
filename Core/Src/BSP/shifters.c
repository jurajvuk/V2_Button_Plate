
/*********************************INCLUDES******************************************/
#include "BSP/shifters.h"
#include "BSP/calibration.h"
#include "BSP/adc_input.h"
#include "BSP/usb_hid_report.h"

/**********************GLOBAL VARIABLES AND CONSTANTS*******************************/
volatile uint16_t shifter_attack_point[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Attack points for each shifter
volatile int16_t shifter_magnet_orientation[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/****************************FORWARD DECLARATIONS***********************************/


/*********************************FUNCTIONS*****************************************/
void shifter_init(void)
{
    for (int i = 0; i < 7; i = i + 2) 
    {
        if (g_latestCalibrationReport.minMaxCalibrationValues[i + 1] > g_latestCalibrationReport.minMaxCalibrationValues[i] + SHIFTER_LIMIT) 
        {
            shifter_attack_point[i] = SHIFTER_ATTACK_POINT * (g_latestCalibrationReport.minMaxCalibrationValues[i + 1] - g_latestCalibrationReport.minMaxCalibrationValues[i]);
            shifter_magnet_orientation[i] = 1;
        } 
        else if (g_latestCalibrationReport.minMaxCalibrationValues[i + 1] < g_latestCalibrationReport.minMaxCalibrationValues[i] - SHIFTER_LIMIT) 
        {
            shifter_attack_point[i] = SHIFTER_ATTACK_POINT * (g_latestCalibrationReport.minMaxCalibrationValues[i] - g_latestCalibrationReport.minMaxCalibrationValues[i + 1]);
            shifter_magnet_orientation[i] = -1;
        }
    }
}

void shifter_update(void)
{
    for (int i = 0; i < 4; i++) 
    {
        if (shifter_attack_point[i*2] < ((adc_update_input[i] - g_latestCalibrationReport.minMaxCalibrationValues[i*2])* shifter_magnet_orientation[i*2]))
        {
            HID_InputReport.buttons[SHIFTER_INDEX] |= (1 << i); // Set the shifter when pressed
        } 
        else 
        {
            HID_InputReport.buttons[SHIFTER_INDEX] &= ~(1 << i); // Clear the shifter when not pressed
        }
        
    }
    
}