#ifndef _DEVICES_H
#define _DEVICES_H

#include "devicelist.h"
#include "params.h"
#include "spi/flash/flash.h"
#include "spi/lora/lora.h"
#include "spi/sensors/accel//kx134_1211.h"
#include "spi/sensors/baro/bmp581.h"
#include "spi/sensors/gyro/a3g4250d.h"
#include "uart/gps/gps.h"
#include "uart/uart.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

bool initDevices();

#define DEVICE_NAME_LENGTH 20
#define STATE_NAME_LENGTH  20

typedef struct StateHandle_t {
  char name[STATE_NAME_LENGTH];
  void *state;
  struct StateHandle_t *ref;
} StateHandle_t;

StateHandle_t StateHandle_getHandle(char *);
StateHandle_t *StateHandle_getHandleRef(char *);

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
#define USB_PINS \
  (UART_Pins) { .TX = 6, .RX = 7 }
#define USB_BAUD 921600

// GPS UART
#define GPS_INTERFACE USART3
#define GPS_PORT      GPIOD
#define GPS_PINS \
  (UART_Pins) { .TX = 8, .RX = 9 }
#define GPS_BAUD 9600

#endif
