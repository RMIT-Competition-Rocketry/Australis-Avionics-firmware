#ifndef AUSTRALISCONFIG_H
#define AUSTRALISCONFIG_H

/* ===================================================================== *
 *                             FLIGHT THRESHOLDS                         *
 * ===================================================================== */

#define MAIN_ALTITUDE_METERS 390.0f
#define ACCEL_LAUNCH         2.0f

/* ===================================================================== *
 *                                DEVICE AXES                            *
 * ===================================================================== */

// Accel 1
#ifdef FLIGHT_TEST
  // DRONE AXES
  #define ACCEL_AXES_1 ((const uint8_t[]){0, 1, 2})
  #define ACCEL_SIGN_1 ((const int8_t[]){1, 1, -1})
#else
  // FLIGHT AXES
  #define ACCEL_AXES_1 ((const uint8_t[]){0, 2, 1})
  #define ACCEL_SIGN_1 ((const int8_t[]){1, 1, -1})
#endif

// Accel 2
#ifdef FLIGHT_TEST
  // DRONE AXES
  #define ACCEL_AXES_2 ((const uint8_t[]){0, 1, 2})
  #define ACCEL_SIGN_2 ((const int8_t[]){1, 1, -1})
#else
  // FLIGHT AXES
  #define ACCEL_AXES_2 ((const uint8_t[]){2, 0, 1})
  #define ACCEL_SIGN_2 ((const int8_t[]){1, -1, 1})
#endif

// Gyroscope
#define GYRO_AXES ((const uint8_t[]){0, 2, 1})
#define GYRO_SIGN ((const int8_t[]){1, 1, 1})

/* ===================================================================== *
 *                               DEVICE SCALE                            *
 * ===================================================================== */

#define ACCEL_SCALE_HIGH 32
#define ACCEL_SCALE_LOW  16

#endif
