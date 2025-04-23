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
#include "packet.h"
#include "state.h"
#include "devicelist.h"
#include "accelerometer.h"
#include "barometer.h"
#include "gyroscope.h"

// TODO: Implement this better
#include "sam_m10q.h"

#include "loracomm.h"

static void sendGroundPacket1(uint8_t broadcastBegin);
static void sendGroundPacket2(SAM_M10Q_Data *data);

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
  gpsSubStateMachine = xQueueCreate(10, sizeof(SAM_M10Q_Data));
  // Struct to store GPS topic articles
  SAM_M10Q_Data gpsData;

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

  // TODO:
  // Remove hard-coded numbers in favour of defined packet
  // and field lengths in header.

  uint8_t bytes[32];

  {
    // --- Construct Packet ---
    /*
     * The packet is initialised as a Packet struct containing
     * all fields necessary for id 0x03.
     *
     * Scope is limited for this struct to reduce the memory footprint,
     * as data is essentially doubled for the duration of the packet
     * construction process.
     * The packet structure itself contains information on the fields,
     * the length of each, as well as the data contained. This data
     * must be copied to a raw byte array (the bytes variable) for
     * transmission.
     */
    Packet packet =
      {
        .id     = 0x03,
        .length = 6,
        .fields = (Field[]){
          {// [0] Flight State
           .size = 1,
           .data = (uint8_t[]){
             state->flightState
           }
          },
          {// [7:2] Low-rate raw accelerometer data
           .size = lAccel->dataSize,
           .data = lAccel->rawAccelData
          },
          {// [13:8] High-rate raw accelerometer data
           .size = hAccel->dataSize,
           .data = hAccel->rawAccelData
          },
          {// [19:14] Raw gyroscope data
           .size = gyro->dataSize,
           .data = gyro->rawGyroData
          },
          {// [23:20] Altitude (big endian)
           .size = 4,
           .data = (uint8_t[]){
             (&state->altitude)[3],
             (&state->altitude)[2],
             (&state->altitude)[1],
             (&state->altitude)[0],
           }
          },
          {// [27:24] Velocity (big endian)
           .size = 4,
           .data = (uint8_t[]){
             (&state->velocity)[3],
             (&state->velocity)[2],
             (&state->velocity)[1],
             (&state->velocity)[0],
           }
          },
          // TODO: [28] Apo continuity
          {.size = 1, .data = (uint8_t[]){}},
          // TODO: [29] Main continuity
          {.size = 1, .data = (uint8_t[]){}},
          {// [30] Broadcast flag
           .size = 1,
           .data = (uint8_t[]){broadcastBegin}
          },
        }
      };

    // Construct byte array from packet structure
    Packet_asBytes(&packet, bytes, 32);
  }

  // Send packet comment to LoRa author
  Topic_comment(loraTopic, bytes);
}

/* =============================================================================== */
/**
 * @brief Send ground data packet 2
 *
 **
 * =============================================================================== */
void sendGroundPacket2(SAM_M10Q_Data *data) {
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
