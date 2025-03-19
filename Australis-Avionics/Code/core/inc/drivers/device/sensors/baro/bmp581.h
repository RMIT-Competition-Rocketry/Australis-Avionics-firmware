/**
 * @author Matt Ricci
 * @ingroup Sensors
 * @addtogroup Barometer
 * @file BMP581.h
 * @todo Document interface
 */

#ifndef _BMP581_H
#define _BMP581_H

#include "stm32f439xx.h"

#include "spi.h"
#include "gpiopin.h"

#define BMP581_TEMP_SENSITIVITY       (1.0f / 65535)
#define BMP581_PRESS_SENSITIVITY      (1.0f / 64)
#define BMP581_CHIP_ID                0x01
#define BMP581_ODR_CFG                0x37
#define BMP581_ODR_CFG_PWR            0x03
#define BMP581_ODR_CFG_PWR_STANDBY    0x00
#define BMP581_ODR_CFG_PWR_CONTINUOUS 0x03
#define BMP581_ODR_CFG_DEEP_DIS       0x80
#define BMP581_OSR_CFG_RESERVED       0x80
#define BMP581_OSR_CFG                0x36
#define BMP581_OSR_CFG_OSR_P          0x38
#define BMP581_OSR_CFG_OSR_P_16       0x04
#define BMP581_OSR_CFG_PRESS_EN       0x40
#define BMP581_INT_STATUS             0x27
#define BMP581_STATUS                 0x28
#define BMP581_STATUS_NVM_RDY         0x02
#define BMP581_STATUS_NVM_ERR         0x04
#define BMP581_OSR_CFG_PRESS_EN       0x40
#define BMP581_TEMPERATURE_XLSB       0x1D
#define BMP581_TEMPERATURE_LSB        0x1E
#define BMP581_TEMPERATURE_MSB        0x1F
#define BMP581_PRESSURE_XLSB          0x20
#define BMP581_PRESSURE_LSB           0x21
#define BMP581_PRESSURE_MSB           0x22
#define BMP581_CMD                    0x7E

#define BMP581_DATA_SIZE              3 // Three bytes per reading
#define BMP581_DATA_COUNT             2 // Two readings - temperature, pressure
#define BMP581_DATA_TOTAL             (BMP581_DATA_COUNT * BMP581_DATA_SIZE)

/**
 * @ingroup Barometer
 * @addtogroup BMP581
 * @{
 */

/** @extends SPI */
typedef struct BMP581 {
  SPI_t *base;
  GPIOpin_t cs;
  float pressSensitivity;
  float tempSensitivity;
  void (*update)(struct BMP581 *);
  void (*readTemp)(struct BMP581 *, float *);
  void (*readPress)(struct BMP581 *, float *);
  void (*readRawTemp)(struct BMP581 *, uint8_t *);
  void (*readRawPress)(struct BMP581 *, uint8_t *);
  void (*processRawTemp)(struct BMP581 *, uint8_t *, float *);
  void (*processRawPress)(struct BMP581 *, uint8_t *, float *);
  uint8_t rawTemp[BMP581_DATA_SIZE];
  uint8_t rawPress[BMP581_DATA_SIZE];
  float temp;
  float press;
  float groundPress;
} BMP581_t;

BMP581_t BMP581_init(BMP581_t *, SPI_t *, GPIOpin_t, const float, const float);
void BMP581_update(BMP581_t *);
void BMP581_readTemp(BMP581_t *, float *);
void BMP581_readPress(BMP581_t *, float *);
void BMP581_readRawTemp(BMP581_t *, uint8_t *);
void BMP581_readRawPress(BMP581_t *, uint8_t *);
void BMP581_processRawTemp(BMP581_t *, uint8_t *, float *);
void BMP581_processRawPress(BMP581_t *, uint8_t *, float *);

uint8_t BMP581_readRegister(BMP581_t *, uint8_t);
void BMP581_readRegisters(BMP581_t *, uint8_t, uint8_t, uint8_t *);
void BMP581_writeRegister(BMP581_t *, uint8_t, uint8_t);

/** @} */
#endif
