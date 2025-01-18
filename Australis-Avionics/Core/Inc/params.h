#ifndef _PARAMS_H
#define _PARAMS_H

/* ===================================================================== *
 *                                   CAN                                 *
 * ===================================================================== */
 
#define CAT_CAN_INTF(x)             CAN##x
#define CAN_INTF(INTF)              CAT_CAN_INTF(INTF)

#define CAN_AB                      1
#define CAN_AB_INTF                 CAN_INTF(CAN_AB)
#define CAN_PAYLOAD_AV              2
#define CAN_PAYLOAD_AV_INTF         CAN_INTF(CAN_PAYLOAD_AV)

/* ===================================================================== *
 *                             FLIGHT THRESHOLDS                         *
 * ===================================================================== */

#define ACCEL_LAUNCH         5.0f
#define MAIN_ALTITUDE_METERS 396.0f

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

#endif
