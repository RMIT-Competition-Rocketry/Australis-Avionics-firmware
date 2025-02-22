/**
 * @author Matt Ricci
 */

// ALLOW FORMATTING
#ifndef _DEVICES_H
#define _DEVICES_H

#include "stdbool.h"

bool initSpiBuses();
bool initDevices();

#define STATE_NAME_LENGTH 20

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

// DEVICE SPECIFIC

#define ACCEL_PORT_1     GPIOA                         // Accel 1 CS on GPIOA
#define ACCEL_CS_1       GPIO_ODR_OD1                  // Accel 1 CS on PA1
#define ACCEL_PORT_2     GPIOB                         // Accel 2 CS on port B
#define ACCEL_CS_2       GPIO_ODR_OD0                  // Accel 2 CS on PB0

#define GYRO_PORT        GPIOA                         // Gyro CS on GPIOA
#define GYRO_CS          GPIO_ODR_OD2                  // Gyro CS on PA2

#define BARO_PORT        GPIOA                         // Baro CS on GPIOA
#define BARO_CS          GPIO_ODR_OD3                  // Baro CS on PA3

#define FLASH_PORT       GPIOE                         // Flash CS on GPIOE
#define FLASH_CS         GPIO_ODR_OD11                 // Flash CS on PE11
#define FLASH_PAGE_SIZE  256                           // 256 bytes per page
#define FLASH_PAGE_COUNT 65536                         // 65536 total pages

#define LORA_PORT        GPIOD                         // LoRa CS on GPIOD
#define LORA_CS          GPIO_ODR_OD0                  // LoRa CS on PD0
#define LORA_BW          BW500                         // 500KHz bandwidth
#define LORA_SF          SF9                           // Spreading factor 9
#define LORA_CR          CR5                           // Coding rate 4/5

#define USB_INTERFACE    USART6                        // USB on UART6
#define USB_PORT         GPIOC                         // UART pins on GPIOC
#define USB_PINS         (UART_Pins){.TX = 6, .RX = 7} // TX PC6, RX PC7
#define USB_BAUD         921600                        // 921600bps baud rate
#define USB_OVERSAMPLE   OVER8                         // 8-bit oversampling

#define GPS_INTERFACE    USART3                        // GPS on UART3
#define GPS_PORT         GPIOD                         // UART pins on GPIOD
#define GPS_PINS         (UART_Pins){.TX = 8, .RX = 9} // TX PD8, RX PD9
#define GPS_BAUD         9600                          // 96000bps baud rate

// PERIPHERAL BUS

#define SENSORS_SPI_PORT GPIOA     // Sensor suite SPI bus pins on GPIOA
#define SENSORS_SPI_SCK  GPIO_PIN5 // SCK pin on PA5
#define SENSORS_SPI_SDI  GPIO_PIN6 // SDI pin on PA6
#define SENSORS_SPI_SDO  GPIO_PIN7 // SDO pin on PA7

#endif
