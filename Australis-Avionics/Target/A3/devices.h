/**
 * @author Matt Ricci
 */

// ALLOW FORMATTING
#ifndef _DEVICES_H
#define _DEVICES_H

#include "stdbool.h"

void initSpiPins();
bool initDevices();
bool initSensors();
bool initFlash();
bool initLora();
bool initUart();

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

/*********************  Device specific definitions  *********************/

#define ACCEL_CS_PORT1        GPIOA                         // Accel 1 CS on GPIOA
#define ACCEL_CS_PIN1         GPIO_PIN1                     // Accel 1 CS on PA1
#define ACCEL_CS1             ACCEL_CS_PORT1, ACCEL_CS_PIN1 //

#define ACCEL_CS_PORT2        GPIOB                         // Accel 2 CS on GPIOB
#define ACCEL_CS_PIN2         GPIO_PIN0                     // Accel 2 CS on PB0
#define ACCEL_CS2             ACCEL_CS_PORT2, ACCEL_CS_PIN2 //

#define BARO_CS_PORT          GPIOA                         // Baro CS on GPIOA
#define BARO_CS_PIN           GPIO_PIN3                     // Baro CS on PA2
#define BARO_CS               BARO_CS_PORT, BARO_CS_PIN     //

#define GYRO_CS_PORT          GPIOA                         // Gyro CS on GPIOA
#define GYRO_CS_PIN           GPIO_PIN2                     // Gyro CS on PA2
#define GYRO_CS               GYRO_CS_PORT, GYRO_CS_PIN     //

#define FLASH_CS_PORT         GPIOE                         // Flash CS on GPIOE
#define FLASH_CS_PIN          GPIO_PIN11                    // Flash CS on PE11
#define FLASH_CS              GYRO_CS_PORT, GYRO_CS_PIN     //
#define FLASH_PAGE_SIZE       256                           // 256 bytes per page
#define FLASH_PAGE_COUNT      65536                         // 65536 total pages

#define LORA_CS_PORT          GPIOD                         // LoRa CS on GPIOD
#define LORA_CS_PIN           GPIO_PIN0                     // LoRa CS on PD0
#define LORA_CS               LORA_CS_PORT, LORA_CS_PIN     //
#define LORA_BW               BW500                         // 500KHz bandwidth
#define LORA_SF               SF9                           // Spreading factor 9
#define LORA_CR               CR5                           // Coding rate 4/5

#define USB_INTERFACE         USART6                        // USB on UART6
#define USB_PORT              GPIOC                         // UART pins on GPIOA
#define USB_PINS              (UART_Pins){.TX = 6, .RX = 7} // TX PC6, RX PC7
#define USB_BAUD              921600                        // 921600bps baud rate
#define USB_OVERSAMPLE        OVER8                         // 8-bit oversampling

#define GPS_INTERFACE         USART3                        // GPS on UART3
#define GPS_PORT              GPIOD                         // UART pins on GPIOD
#define GPS_PINS              (UART_Pins){.TX = 8, .RX = 9} // TX PD8, RX PD9
#define GPS_BAUD              9600                          // 96000bps baud rate

/**********************  Peripheral bus definitions  *********************/

#define SENSORS_SPI_INTERFACE SPI1       // Sensor suite SPI bus interface on SPI1
#define SENSORS_SPI_PORT      GPIOA      // Sensor suite SPI bus pins on GPIOA
#define SENSORS_SPI_AF        GPIO_AF5   // Sensor suite SPI pin alternate function
#define SENSORS_SPI_SCK       GPIO_PIN5  // SCK pin on PA5
#define SENSORS_SPI_SDO       GPIO_PIN6  // SDI pin on PA6
#define SENSORS_SPI_SDI       GPIO_PIN7  // SDO pin on PA7

#define FLASH_SPI_INTERFACE   SPI4       // Flash chip SPI bus interface on SPI4
#define FLASH_SPI_PORT        GPIOE      // Flash chip SPI bus pins on GPIOE
#define FLASH_SPI_AF          GPIO_AF5   // Flash chip SPI pin alternate function
#define FLASH_SPI_SCK         GPIO_PIN12 // SCK pin on PE12
#define FLASH_SPI_SDO         GPIO_PIN13 // SDI pin on PE13
#define FLASH_SPI_SDI         GPIO_PIN14 // SDO pin on PE14

#define LORA_SPI_INTERFACE    SPI3       // LoRa chip SPI bus interface on SPI3
#define LORA_SPI_PORT         GPIOC      // LoRa chip SPI bus pins on GPIOC
#define LORA_SPI_AF           GPIO_AF6   // LoRa chip SPI pin alternate function
#define LORA_SPI_SCK          GPIO_PIN10 // SCK pin on PE10
#define LORA_SPI_SDO          GPIO_PIN11 // SDI pin on PE11
#define LORA_SPI_SDI          GPIO_PIN12 // SDO pin on PE12

#endif
