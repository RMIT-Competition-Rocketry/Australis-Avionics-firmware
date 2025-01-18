/**
 * @author Matt Ricci
 * @file   main.h
 **/

#ifndef __MAIN_H
#define __MAIN_H

#include "stdint.h"
#include "stdio.h"
#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "event_groups.h"
#include "groups.h"
#include "handles.h"
#include "message_buffer.h"
#include "semphr.h"

#include "flashWrite.h"
#include "gpsAcquisition.h"
#include "hDataAcquisition.h"
#include "lDataAcquisition.h"

#include "loraComm.h"
#include "payloadComm.h"
#include "usbComm.h"

#include "stateUpdate.h"

#include "A3G4250D.h"
#include "BMP581.h"
#include "KX134_1211.h"
#include "can.h"
#include "dataframe.h"
#include "devices.h"
#include "drivers.h"
#include "flash.h"
#include "gps.h"
#include "lora.h"
#include "sensors.h"
#include "shell.h"
#include "uart.h"

#include "kalmanfilter.h"
#include "membuff.h"
#include "quaternion.h"
#include "slidingwindow.h"

#ifdef DUMMY
  #include "accelX.h"
  #include "accelY.h"
  #include "accelZ.h"
  #include "gyroX.h"
  #include "gyroY.h"
  #include "gyroZ.h"
  #include "press.h"
#endif

void vSystemInit(void *pvParameters);
void deviceInit();
void configure_interrupts();
void Error_Handler(void);

#define AVG_BUFF_SIZE  15
#define LORA_BUFF_SIZE 128
#define MEM_BUFF_SIZE  20992

/* ===================================================================== *
 *                           DEVICE DEFINITIONS                          *
 * ===================================================================== */

// ACCELEROMETER
#define ACCEL_PORT_1 GPIOA
#define ACCEL_CS_1   GPIO_ODR_OD1
#define ACCEL_PORT_2 GPIOB
#define ACCEL_CS_2   GPIO_ODR_OD0

// GYROSCOPE
#define GYRO_PORT GPIOA
#define GYRO_CS   GPIO_ODR_OD2

// BAROMETER
#define BARO_PORT GPIOA
#define BARO_CS   GPIO_ODR_OD3

// FLASH
#define FLASH_PORT       GPIOE
#define FLASH_CS         GPIO_ODR_OD11
#define FLASH_PAGE_SIZE  256
#define FLASH_PAGE_COUNT 65536

// LORA
#define LORA_PORT GPIOD
#define LORA_CS   GPIO_ODR_OD0

// USB UART
#define USB_INTERFACE USART6
#define USB_PORT      GPIOC
#define USB_PINS 			(UART_Pins){.TX = 6, .RX = 7}
#define USB_BAUD      921600

// GPS UART
#define GPS_INTERFACE USART3
#define GPS_PORT      GPIOD
#define GPS_PINS 			(UART_Pins){.TX = 8, .RX = 9}
#define GPS_BAUD      9600

#endif
