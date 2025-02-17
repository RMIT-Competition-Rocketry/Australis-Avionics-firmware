/**
 * @author Matt Ricci
 * @ingroup Sensors
 * @addtogroup Accelerometer
 * @file KX134_1211.h
 */

#ifndef _KX134_1211_H
#define _KX134_1211_H

#include "stm32f439xx.h"
#include "string.h"

#include "devicelist.h"
#include "spi.h"

#define KX134_1211_SENSITIVITY_32G    (1.0f / 1024.0f)
#define KX134_1211_SENSITIVITY_16G    (1.0f / 2048.0f)
#define KX134_1211_SENSITIVITY(scale) KX134_1211_SENSITIVITY_##scale##G
#define KX134_1211_CNTL1              0x1B
#define KX134_1211_CNTL1_PC1          0x80
#define KX134_1211_CNTL1_DRDYE        0x20
#define KX134_1211_CNTL1_RES          0x40
#define KX134_1211_CNTL1_GSEL_32G     0x10
#define KX134_1211_CNTL1_GSEL_16G     0x08
#define KX134_1211_CNTL1_GSEL_8G      0x00
#define KX134_1211_CNTL1_GSEL(scale)  KX134_1211_CNTL1_GSEL_##scale##G
#define KX134_1211_ODCNTL             0x21
#define KX134_1211_ODCNTL_RESERVED    0x90
#define KX134_1211_XOUT_L             0x08
#define KX134_1211_XOUT_H             0x09
#define KX134_1211_YOUT_L             0x0A
#define KX134_1211_YOUT_H             0x0B
#define KX134_1211_ZOUT_L             0x0C
#define KX134_1211_ZOUT_H             0x0D
#define KX134_1211_INS2               0x17

#define KX134_1211_INS2_DRDY          0x10

#define KX134_1211_DATA_SIZE          2 // Two bytes per axis
#define KX134_1211_DATA_COUNT         3 // Three axes - X Y Z
#define KX134_1211_DATA_TOTAL         (KX134_1211_DATA_COUNT * KX134_1211_DATA_SIZE)

/**
 * @ingroup Accelerometer
 * @defgroup KX134-1211
 * @addtogroup KX134-1211
 * @{
 */

/** @extends SPI */
typedef struct KX134_1211 {
  SPI base;                                                         //!< Parent SPI interface
  float sensitivity;                                                //!< Accelerometer sensitivity
  void (*update)(struct KX134_1211 *);                              //!< Accel update method.       @see KX134_1211_update
  void (*readAccel)(struct KX134_1211 *, float *);                  //!< Accel read method.         @see KX134_1211_readAccel
  void (*readRawBytes)(struct KX134_1211 *, uint8_t *);             //!< Raw accel read method.     @see KX134_1211_readRawBytes
  void (*processRawBytes)(struct KX134_1211 *, uint8_t *, float *); //!< Process raw accel method.  @see KX134_1211_processRawBytes
  uint8_t axes[KX134_1211_DATA_COUNT];                              //!< Array defining axes of mounting
  int8_t sign[KX134_1211_DATA_COUNT];                               //!< Array defining sign of axes
  uint8_t rawAccelData[KX134_1211_DATA_TOTAL];                      //!< Raw accelerations array
  float accelData[KX134_1211_DATA_COUNT];                           //!< Processed accelerations array
} KX134_1211;

DeviceHandle_t KX134_1211_init(KX134_1211 *, char *, GPIO_TypeDef *, unsigned long, const uint8_t, const uint8_t *, const int8_t *);
void KX134_1211_update(KX134_1211 *);
void KX134_1211_readAccel(KX134_1211 *, float *);
void KX134_1211_readRawBytes(KX134_1211 *, uint8_t *);
void KX134_1211_processRawBytes(KX134_1211 *, uint8_t *, float *);

void KX134_1211_readRegisters(KX134_1211 *, uint8_t, uint8_t, uint8_t *);
uint8_t KX134_1211_readRegister(KX134_1211 *, uint8_t);
void KX134_1211_writeRegister(KX134_1211 *, uint8_t, uint8_t);

/** @} */
#endif
