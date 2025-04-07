/***********************************************************************************
 * @file  groundcomms.c                                                            *
 * @brief                                                                          *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "groundcomms.h"

#include "stdbool.h"

#include "FreeRTOS.h"
#include "queue.h"

#include "topic.h"
#include "state.h"
#include "devicelist.h"
#include "accelerometer.h"
#include "barometer.h"
#include "gyroscope.h"

#include "loracomm.h"

static void sendGroundPacket1();

/* =============================================================================== */
/**
 * @brief AV to ground communications state machine.
 *
 **
 * =============================================================================== */
void vGroundCommStateMachine(void *argument) {
  const TickType_t xFrequency = pdMS_TO_TICKS(250);

  // Create subscription to LoRa topic
  static SUBSCRIBE_TOPIC(lora, loraSubStateMachine);
  loraSubStateMachine = xQueueCreate(10, LORA_MSG_LENGTH);
  // Binary array to store topic articles
  uint8_t loraRxData[LORA_MSG_LENGTH];

  // Create subscription to GPS topic
  static SUBSCRIBE_TOPIC(gps, gpsSubStateMachine);
  // TODO:
  // Replace this queue initialisation with something
  // more sensible for the GPS data
  gpsSubStateMachine = xQueueCreate(10, 128);
  // Binary array to store topic articles
  uint8_t gpsRxData[128];

  State *state        = State_getState();
  bool broadcastBegin = false;

  for (;;) {

    // Determine action based on the current flight state
    switch (state->flightState) {

    // --- Ground Pre-Launch State ---
    case PRELAUNCH:
      /*
       * PRELAUNCH State: Wait for a specific request from the GCS.
       *
       * In this state, the system blocks indefinitely until it receives a
       * message on the LoRa queue with the specific GCS Request ID.
       */
      if (xQueueReceive(loraSubStateMachine, loraRxData, xFrequency)) {
        // Check if the received message has the expected GCS Request ID
        if (loraRxData[LORA_MESSAGE_INDEX_ID] == LORA_MESSAGE_ID_GCS_REQUEST) {
          // Send response back to the ground
          sendGroundPacket1();
        }
        // else: Received a message, but not the one we were waiting for. Ignore it in prelaunch.
      }
      break; // Exit PRELAUNCH state handling

    // --- Active Flight States ---
    // These states share similar communication logic: either wait for the
    // broadcast start command or continuously send data if broadcasting started.
    case LAUNCH:
    case COAST:
    case APOGEE:
    case DESCENT:
      /*
       * Active Flight States (LAUNCH, COAST, APOGEE, DESCENT):
       *
       * Handles communication during active flight phases.
       * 1. If broadcast hasn't started: Waits for a specific command from GCS
       *    to begin broadcasting telemetry.
       * 2. If broadcast has started: Continuously sends telemetry data to GCS.
       */
      if (!broadcastBegin) {
        // --- Wait for the "Start Broadcast" command from GCS ---
        // Block forever waiting for a message on the LoRa receive queue
        if (xQueueReceive(loraSubStateMachine, loraRxData, xFrequency)) {
          // Check if it's the GCS command (ID match) AND the broadcast flag is set
          if ((loraRxData[LORA_MESSAGE_INDEX_ID] == LORA_MESSAGE_ID_GCS_REQUEST) // Check if packet has GCS request ID
              && (loraRxData[LORA_MESSAGE_INDEX_BCAST_FLAG] != 0)) {             // Check if broadcast flag byte is non-zero
            // Update broadcast flag
            broadcastBegin = true;
          }
          // else: Received something else, ignore it until broadcast starts.
        }
      } else {
        // --- Broadcast Telemetry ---
        // Broadcast has already been started, continuously send data
        sendGroundPacket1();
      }
      break; // Exit active flight state handling (common break for LAUNCH to DESCENT)
    }
  }
}

/* =============================================================================== */
/**
 * @brief Send ground data packet 1
 *
 **
 * =============================================================================== */
void sendGroundPacket1() {
  Accel_t *lAccel = DeviceList_getDeviceHandle(DEVICE_ACCEL_LOW).device;
  Accel_t *hAccel = DeviceList_getDeviceHandle(DEVICE_ACCEL_HIGH).device;
  Baro_t *baro    = DeviceList_getDeviceHandle(DEVICE_BARO).device;
  Gyro_t *gyro    = DeviceList_getDeviceHandle(DEVICE_GYRO).device;

  State *state    = State_getState();

  uint8_t packet[LORA_MSG_LENGTH];
  int idx = 0;

  // Construct packet from state and device data
  //
  //  Index : Name            | Axis | Endian
  //  --------------------------------------------
  //  0     : ID (0x03)       |  -   | -
  //  1     : State           |  -   | -
  //  2-3   : Low Accel       |  X   | (MSB first)
  //  4-5   : Low Accel       |  Y   | (MSB first)
  //  6-7   : Low Accel       |  Z   | (MSB first)
  //  8-9   : High Accel      |  X   | (MSB first)
  //  10-11 : High Accel      |  Y   | (MSB first)
  //  12-13 : High Accel      |  Z   | (MSB first)
  //  14-15 : Gyro            |  X   | (MSB first)
  //  16-17 : Gyro            |  Y   | (MSB first)
  //  18-19 : Gyro            |  Z   | (MSB first)
  //  20-23 : Altitude        |  -   | (MSB first)
  //  24-27 : Velocity        |  -   | (MSB first)
  //  28    : Apo continuity  |  -   | -
  //  29    : Main continuity |  -   | -
  //  30    : Broadcast start |  -   | -
  //  --------------------------------------------

  packet[idx++] = 0x03;
  packet[idx++] = state->flightState;
  memcpy(&packet[idx], lAccel->rawAccelData, lAccel->dataSize);
  memcpy(&packet[idx += lAccel->dataSize], hAccel->rawAccelData, hAccel->dataSize);
  memcpy(&packet[idx += hAccel->dataSize], gyro->rawGyroData, gyro->dataSize);
  memcpy(&packet[idx += gyro->dataSize], &state->altitude, sizeof(float));
  memcpy(&packet[idx += sizeof(float)], &state->velocity, sizeof(float));

  // Send packet comment to LoRa author
  Topic_comment(loraTopic, packet);
}

/** @} */
