// ALLOW FORMATTING
#ifndef GYRO_H
#define GYRO_H

#include "stdint.h"

typedef struct Gyro {
  uint8_t dataSize;                                           //!< Total data size.
  uint8_t *axes;                                              //!< Array defining axes of mounting
  int8_t *sign;                                               //!< Array defining sign of axes
  void (*update)(struct Gyro *);                              //!< Gyro update method.
  void (*readGyro)(struct Gyro *, float *);                   //!< Gyro read method.
  void (*readRawBytes)(struct Gyro *, uint8_t *);             //!< Raw accel read method.
  void (*processRawBytes)(struct Gyro *, uint8_t *, float *); //!< Process raw accel method.
  uint8_t *rawGyroData;                                       //!< Raw accelerations array
  float *gyroData;                                            //!< Processed accelerations array
} Gyro_t;

#endif
