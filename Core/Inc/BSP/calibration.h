#ifndef CALIBRATION_H_
#define CALIBRATION_H_

/*********************************INCLUDES******************************************/
#include "stm32f1xx_hal.h"

/**********************************DEFINES******************************************/
#define CALIBRATION_DATA_FLASH_ADDRESS   0x0800FC00 // Address in Flash memory where calibration data is stored

/*********************************STRUCTURES****************************************/
// --- CORRECTED Struct with Padding ---
#if defined(__GNUC__)
  #define PACKED   __attribute__((packed))
#else
  #define PACKED
  #pragma pack(push,1)
#endif

// — Report ID 1: 40 buttons + 6×12-bit axes (14 bytes payload + 1 ID)
typedef struct PACKED {
    uint8_t  reportID;       // == 4
    uint8_t  calibrationStart;
    uint8_t axisIndex;
    uint8_t  padding;
    // 12 axes, each 12-bit little-endian; we store in 16-bit slots:
    uint16_t minMaxCalibrationValues[12];        // only lower 12 bits of each are valid
} HID_CalibrationReport_t;         // sizeof = 1 + 5 + (6×2) = 18 bytes

#if !defined(__GNUC__)
  #pragma pack(pop)
#endif
/******************************EXTERN VARIABLES*************************************/
extern volatile HID_CalibrationReport_t g_received_report_id_4;
extern volatile HID_CalibrationReport_t g_latestCalibrationReport;

/****************************FUNCTION PROTOTYPES************************************/
void calibration_init(void);
void calibration_update(void);
void Save_Calibration_Data(void);
void Load_Calibration_Data(void);

#endif /* CALIBRATION_H_ */
