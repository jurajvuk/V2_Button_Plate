/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v2.0_Cube
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */
#include "BSP/calibration.h"
#include "BSP/ws2812b.h"
#include "stm32f1xx_it.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */
  0x05, 0x01,                    // Usage Page (Generic Desktop)
  0x09, 0x05,                    // Usage (Gamepad)
  0xA1, 0x01,                    // Collection (Application)

  // ===================================================================
  // Report ID 1: 64 Buttons + Padding + 8x16-bit Axes
  // Total size: 1 (ID) + 8 (Buttons) + 1 (Padding) + 16 (Axes) = 26 bytes
  // ===================================================================
  0x85, 0x01,                    //   Report ID (1)

  // 64 Buttons (8 bytes)
  0x05, 0x09,                    //   Usage Page (Button)
  0x19, 0x01,                    //   Usage Minimum (1)
  0x29, 0x40,                    //   Usage Maximum (64)
  0x15, 0x00,                    //   Logical Minimum (0)
  0x25, 0x01,                    //   Logical Maximum (1)
  0x75, 0x01,                    //   Report Size (1)
  0x95, 0x40,                    //   Report Count (64)
  0x81, 0x02,                    //   Input (Data,Var,Abs)

  // Alignment Padding (1 byte)
  // This pushes the axes to start at byte 10 (buffer[10]) in the HID report
  0x75, 0x08,                    //   Report Size (8 bits)
  0x95, 0x01,                    //   Report Count (1)
  0x81, 0x03,                    //   Input (Cnst,Var,Abs) -> Constant padding

  // 8 x 16-bit Analog Axes (16 bytes)
  0x05, 0x01,                    //   Usage Page (Generic Desktop)
  0x09, 0x30,                    //     Usage (X)
  0x09, 0x31,                    //     Usage (Y)
  0x09, 0x32,                    //     Usage (Z)
  0x09, 0x33,                    //     Usage (Rx)
  0x09, 0x34,                    //     Usage (Ry)
  0x09, 0x35,                    //     Usage (Rz)
  0x09, 0x36,                    //     Usage (Slider)
  0x09, 0x37,                    //     Usage (Dial)
  0x15, 0x00,                    //   Logical Minimum (0)
  0x26, 0xFF, 0x0F,              //   Logical Maximum (4095)
  0x75, 0x10,                    //   Report Size (16)
  0x95, 0x08,                    //   Report Count (8)
  0x81, 0x02,                    //   Input (Data,Var,Abs)

  // ===================================================================
  // Report ID 3: SimHub Standard HID LED Protocol
  // Total size: 1 (ID) + 3 (Header) + 60 (RGB Data) = 64 bytes
  // Supports 52 LEDs across 3 packets per frame
  // ===================================================================
  0x85, 0x03,                    //   Report ID (3)
  0x06, 0x02, 0xFF,              //   Usage Page (Vendor-Defined 2)
 
  // Start LED Index (1 byte) — which LED to begin writing at
  0x09, 0x21,                    //   Usage (Vendor Usage 0x21)
  0x15, 0x00,                    //   Logical Minimum (0)
  0x26, 0xFF, 0x00,              //   Logical Maximum (255)
  0x75, 0x08,                    //   Report Size (8)
  0x95, 0x01,                    //   Report Count (1)
  0x91, 0x02,                    //   Output (Data,Var,Abs)
 
  // Affected LED Count (1 byte) — how many LEDs in this packet
  0x09, 0x22,                    //   Usage (Vendor Usage 0x22)
  0x15, 0x00,                    //   Logical Minimum (0)
  0x26, 0xFF, 0x00,              //   Logical Maximum (255)
  0x75, 0x08,                    //   Report Size (8)
  0x95, 0x01,                    //   Report Count (1)
  0x91, 0x02,                    //   Output (Data,Var,Abs)
 
  // Draw Flag (1 byte)
  //   0 = more packets coming
  //   1 = last packet → trigger LED refresh
  //   2 = SimHub connected (on connect report)
  //   3 = SimHub disconnecting (on disconnect report)
  0x09, 0x23,                    //   Usage (Vendor Usage 0x23)
  0x15, 0x00,                    //   Logical Minimum (0)
  0x26, 0xFF, 0x00,              //   Logical Maximum (255)
  0x75, 0x08,                    //   Report Size (8)
  0x95, 0x01,                    //   Report Count (1)
  0x91, 0x02,                    //   Output (Data,Var,Abs)
 
  // LED RGB Data (60 bytes = 20 LEDs max per packet)
  0x09, 0x24,                    //   Usage (Vendor Usage 0x24)
  0x15, 0x00,                    //   Logical Minimum (0)
  0x26, 0xFF, 0x00,              //   Logical Maximum (255)
  0x75, 0x08,                    //   Report Size (8)
  0x95, 0x3C,                    //   Report Count (60)
  0x91, 0x02,                    //   Output (Data,Var,Abs)

  // ===================================================================
  // Report ID 4: Calibration Data
  // Total size: 1 (ID) + 3 (Control) + 24 (Values) = 28 bytes
  // ===================================================================
  0x85, 0x04,                    //   Report ID (4)
  0x06, 0x04, 0xFF,              //   Usage Page (Vendor-Defined 4)

  // Control Bytes (3 bytes: startCal, axisIndex, padding)
  0x09, 0x22,                    //   Usage (CalData_Controls)
  0x15, 0x00,                    //   Logical Minimum (0)
  0x25, 0xFF,                    //   Logical Maximum (255)
  0x75, 0x08,                    //   Report Size (8)
  0x95, 0x03,                    //   Report Count (3)
  0x91, 0x02,                    //   Output (Data,Var,Abs)

  // Calibration Values (12 x 16-bit: 6 min + 6 max)
  0x09, 0x23,                    //   Usage (CalData_Values)
  0x15, 0x00,                    //   Logical Minimum (0)
  0x26, 0xFF, 0xFF,              //   Logical Maximum (65535)
  0x75, 0x10,                    //   Report Size (16)
  0x95, 0x0C,                    //   Report Count (12)
  0x91, 0x02,                    //   Output (Data,Var,Abs)

  /* USER CODE END 0 */
  0xC0    /*     END_COLLECTION	             */
};

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state);

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
  /* USER CODE BEGIN 6 */
  // Get a handle to the HID class data
  USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)hUsbDeviceFS.pClassData;

  // The received data from the PC is now in the buffer: hhid->Report_buf
  // The first byte is the Report ID.
  uint8_t report_id = hhid->Report_buf[0];

  // Get the size of the received report from the USB handle
  uint32_t received_size = USBD_GetRxCount(&hUsbDeviceFS, CUSTOM_HID_EPOUT_ADDR);


  // Check which report was received
  if (report_id == 0x03) // Report ID 3 — SimHub Standard HID LED Protocol
  {
      uint8_t start_led  = hhid->Report_buf[1];  // First LED index to update
      uint8_t led_count  = hhid->Report_buf[2];   // Number of LEDs in this packet
      uint8_t draw_flag  = hhid->Report_buf[3];   // 0=more, 1=last, 2=connect, 3=disconnect
 
      // Handle connect/disconnect signals
      if (draw_flag == 2)
      {
          // SimHub just connected
          g_simhub_connected = 1;
          simhub_keepalive_counter = 0;
          return (USBD_OK);
      }
      else if (draw_flag == 3)
      {
          // SimHub is disconnecting — turn off all LEDs
          g_simhub_connected = 0;
          memset(led_buffer, 0, sizeof(led_buffer));
          g_flag_led_update = 1;
          return (USBD_OK);
      }
 
      // Copy RGB data into LED buffer
      // SimHub sends RGB order — ws2812_update() handles RGB→GRB conversion
      for (uint8_t i = 0; i < led_count && (start_led + i) < NUM_OF_LEDS; i++)
      {
          uint8_t offset = 4 + (i * 3);  // RGB data starts at byte 4
          led_buffer[start_led + i].r = hhid->Report_buf[offset];
          led_buffer[start_led + i].g = hhid->Report_buf[offset + 1];
          led_buffer[start_led + i].b = hhid->Report_buf[offset + 2];
      }
 
      // Draw flag 1 = last packet in frame → trigger LED update
      if (draw_flag == 1)
      {
          g_flag_led_update = 1;
          simhub_keepalive_counter = 0;  // Reset keepalive timeout
      }
  }
  else if (report_id == 0x04) // Report ID 4 is for Calibration data
  {
    
    if (received_size == sizeof(HID_CalibrationReport_t)) 
    {
      memcpy(
          (void*)&g_received_report_id_4, // Destination: your global variable
          (void*)hhid->Report_buf,      // Source: the USB buffer
          sizeof(HID_CalibrationReport_t) // Size: the exact size of your struct
      );
    }
    
  }

  return (USBD_OK);
  /* USER CODE END 6 */
}

/* USER CODE BEGIN 7 */
/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}
*/
/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

