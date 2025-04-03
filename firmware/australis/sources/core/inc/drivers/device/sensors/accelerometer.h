/**
 * @author Matt Ricci
 * @ingroup API
 * @addtogroup Sensors
 */

// ALLOW FORMATTING
#ifndef ACCEL_H
#define ACCEL_H

#include "stdint.h"

/**
 * @ingroup Sensors
 * @addtogroup Accelerometer
 * @{
 */

typedef struct Accel {
  uint8_t dataSize;                                            //!< Total data size.
  uint8_t *axes;                                               //!< Pointer to driver defined axes
  int8_t *sign;                                                //!< Pointer to driver defined signs
  uint8_t *rawAccelData;                                       //!< Pointer to driver defined raw data array
  float *accelData;                                            //!< Pointer to driver defined data array
  void (*update)(struct Accel *);                              //!< Accel update method.
  void (*readAccel)(struct Accel *, float *);                  //!< Accel read method.
  void (*readRawBytes)(struct Accel *, uint8_t *);             //!< Raw accel read method.
  void (*processRawBytes)(struct Accel *, uint8_t *, float *); //!< Process raw accel method.
} Accel_t;

/** @} */
#endif
