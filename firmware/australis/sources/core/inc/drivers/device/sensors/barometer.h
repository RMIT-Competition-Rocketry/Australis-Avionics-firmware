/**
 * @file barometer.h
 * @brief Defines the API for the Barometer sensor.
 * @addtogroup API_baro
 * @{
 */

// ALLOW FORMATTING
#ifndef BARO_H
#define BARO_H

#include "stdint.h"

typedef struct Baro {
  uint8_t pressDataSize;
  uint8_t tempDataSize;
  void (*update)(struct Baro *);
  void (*readTemp)(struct Baro *, float *);
  void (*readPress)(struct Baro *, float *);
  void (*readRawTemp)(struct Baro *, uint8_t *);
  void (*readRawPress)(struct Baro *, uint8_t *);
  void (*processRawTemp)(struct Baro *, uint8_t *, float *);
  void (*processRawPress)(struct Baro *, uint8_t *, float *);
  uint8_t *rawPress;
  uint8_t *rawTemp;
  float groundPress;
  float press;
  float temp;
} Baro_t;

/** @} */
#endif
