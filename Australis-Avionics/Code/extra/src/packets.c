/***********************************************************************************
 * @file        packets.c                                                          *
 * @author      Matt Ricci                                                         *
 * @brief       .                                                                  *
 *                                                                                 *
 ***********************************************************************************/

#include "packets.h"

#include "string.h"

SX1272_Packet SX1272_AVData(
    uint8_t id,
    uint8_t currentState,
    uint8_t *lAccelData,
    uint8_t *hAccelData,
    uint8_t lenAccel,
    uint8_t *gyroData,
    uint8_t lenGyro,
    float altitude,
    float velocity
) {
  SX1272_Packet msg;

  // Convert altitude float to byte array
  union {
    float f;
    uint8_t b[4];
  } a;
  a.f = altitude;

  // Convert velocity float to byte array
  union {
    float f;
    uint8_t b[4];
  } v;
  v.f     = velocity;

  int idx = 0;
  // Append to struct data array
  msg.id          = id;
  msg.data[idx++] = currentState;
  memcpy(&msg.data[idx], lAccelData, lenAccel);
  memcpy(&msg.data[idx += lenAccel], hAccelData, lenAccel);
  memcpy(&msg.data[idx += lenAccel], gyroData, lenGyro);
  memcpy(&msg.data[idx += lenGyro], a.b, sizeof(float));
  memcpy(&msg.data[idx += sizeof(float)], v.b, sizeof(float));

  return msg;
}

SX1272_Packet SX1272_GPSData(
    uint8_t id,
    char *latitude,
    char *longitude,
    uint8_t flags
) {
  SX1272_Packet msg;

  int idx = 0;
  // Append to struct data array
  msg.id = id;
  memcpy(&msg.data[idx], latitude, 15); //!< @todo Move magic number to definition/parameter
  memcpy(&msg.data[idx += 15], longitude, 15);
  msg.data[idx += 15] = flags;

  return msg;
}

SX1272_Packet SX1272_PayloadData(
    uint8_t id,
    uint8_t state,
    uint8_t *accelData,
    uint8_t lenAccelData
) {
  SX1272_Packet msg;

  int idx = 0;
  // Append to struct data array
  msg.id          = id;
  msg.data[idx++] = state;
  memcpy(&msg.data[idx + lenAccelData], accelData, lenAccelData);

  return msg;
}
