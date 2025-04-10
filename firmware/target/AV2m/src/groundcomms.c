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

// TODO: Implement this better
#include "sam_m10q.h"

#include "loracomm.h"

static void sendGroundPacket1(uint8_t broadcastBegin);
static void sendGroundPacket2(GPS_Data *data);

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
  // Binary array to store LoRa topic articles
  uint8_t loraRxData[LORA_MSG_LENGTH];

  // Create subscription to GPS topic
  static SUBSCRIBE_TOPIC(gps, gpsSubStateMachine);
  gpsSubStateMachine = xQueueCreate(10, sizeof(GPS_Data));
  // Struct to store GPS topic articles
  GPS_Data gpsData;

  State *state          = State_getState();
  uint8_t broadcastFlag = 0;

  for (;;) {

    TickType_t xLastWakeTime = xTaskGetTickCount();

    // Determine action based on the current flight state
    switch (state->flightState) {

    // --- Ground Pre-Launch State ---
    /*
     * PRELAUNCH State: Wait for a specific request from the GCS.
     *
     * In this state, the system blocks indefinitely until it receives a
     * message on the LoRa queue with the specific GCS Request ID.
     */
    case PRELAUNCH:
      // --- Wait For Request ---
      // Block the state machine until a valid request is received from the ground.
      // Unwanted packets are filtered out and ignored.
      if (xQueueReceive(loraSubStateMachine, loraRxData, xFrequency)) {
        // Check if the received message has the expected GCS Request ID
        if (loraRxData[LORA_MESSAGE_INDEX_ID] != LORA_MESSAGE_ID_GCS_REQUEST)
          // Do not process if packet is invalid
          continue;

        // --- Transmit AV Data ---
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(380));
        sendGroundPacket1(broadcastFlag);

        // Wait to receive data on GPS topic
        xQueueReceive(gpsSubStateMachine, &gpsData, xFrequency);

        // --- Transmit GPS Data ---
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(380));
        sendGroundPacket2(&gpsData);
      }
      break;

    // --- Active Flight States ---
    /*
     * Active Flight States (LAUNCH, COAST, APOGEE, DESCENT):
     *
     * Handles communication during active flight phases.
     * 1. If broadcast hasn't started: Waits for a specific command from GCS
     *    to begin broadcasting telemetry.
     * 2. If broadcast has started: Continuously sends telemetry data to GCS.
     */
    case LAUNCH:
    case COAST:
    case APOGEE:
    case DESCENT:
      if (broadcastFlag) {
        // --- Broadcast Telemetry ---
        // Broadcast has already been started, continuously send data
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(380));
        sendGroundPacket1(broadcastFlag);
        if (xQueueReceive(gpsSubStateMachine, &gpsData, xFrequency)) {
          // Send response back to the ground
          vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(380));
          sendGroundPacket2(&gpsData);
        }
      }

      else {
        // --- Wait for the "Start Broadcast" command from GCS ---
        // Block forever waiting for a message on the LoRa receive queue
        if (xQueueReceive(loraSubStateMachine, loraRxData, xFrequency)) {
          // Check if it's the GCS command (ID match) AND the broadcast flag is set
          if ((loraRxData[LORA_MESSAGE_INDEX_ID] == LORA_MESSAGE_ID_GCS_REQUEST) // Check if packet has GCS request ID
              && (loraRxData[LORA_MESSAGE_INDEX_BCAST_FLAG] != 0)) {             // Check if broadcast flag byte is non-zero
            // Update broadcast flag
            broadcastFlag = LORA_MESSAGE_BCAST_FLAG;
          }
        }
      }
      break;
    }
  }
}

/* =============================================================================== */
/**
 * @brief Send ground data packet 1
 *
 **
 * =============================================================================== */
void sendGroundPacket1(uint8_t broadcastBegin) {

  Accel_t *lAccel = DeviceList_getDeviceHandle(DEVICE_ACCEL_LOW).device;
  Accel_t *hAccel = DeviceList_getDeviceHandle(DEVICE_ACCEL_HIGH).device;
  Baro_t *baro    = DeviceList_getDeviceHandle(DEVICE_BARO).device;
  Gyro_t *gyro    = DeviceList_getDeviceHandle(DEVICE_GYRO).device;
  State *state    = State_getState();

  uint8_t packet[LORA_MSG_LENGTH];
  int idx = 0;

  /* Construct AV data #1 packet */

  packet[idx++] = 0x03;
  packet[idx++] = state->flightState;

  // Low rate accel data
  memcpy(&packet[idx], lAccel->rawAccelData, lAccel->dataSize);
  idx += lAccel->dataSize;

  // High rate accel data
  memcpy(&packet[idx], hAccel->rawAccelData, hAccel->dataSize);
  idx += hAccel->dataSize;

  // Gyro data
  memcpy(&packet[idx], gyro->rawGyroData, gyro->dataSize);
  idx += gyro->dataSize;

  // Altitude (big endian)
  packet[idx++] = (&state->altitude)[3];
  packet[idx++] = (&state->altitude)[2];
  packet[idx++] = (&state->altitude)[1];
  packet[idx++] = (&state->altitude)[0];

  // Velocity (big endian)
  packet[idx++] = (&state->velocity)[3];
  packet[idx++] = (&state->velocity)[2];
  packet[idx++] = (&state->velocity)[1];
  packet[idx++] = (&state->velocity)[0];

  // TODO:
  //  28    : Apo continuity
  packet[idx++] = 0x00;

  // TODO:
  //  29    : Main continuity
  packet[idx++] = 0x00;

  packet[idx++] = broadcastBegin;

  // Send packet comment to LoRa author
  Topic_comment(loraTopic, packet);
}

/* =============================================================================== */
/**
 * @brief Send ground data packet 2
 *
 **
 * =============================================================================== */
void sendGroundPacket2(GPS_Data *data) {
  uint8_t packet[LORA_MSG_LENGTH];
  int idx       = 0;
  packet[idx++] = 0x04;

  memcpy(&packet[idx], data->latitude, sizeof(data->latitude));
  idx += sizeof(data->latitude);

  memcpy(&packet[idx], data->longitude, sizeof(data->longitude));
  idx += sizeof(data->longitude);

  // Send packet comment to LoRa author
  Topic_comment(loraTopic, packet);
}

/** @} */
