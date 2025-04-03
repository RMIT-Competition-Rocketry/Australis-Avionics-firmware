/***********************************************************************************
 * @file        stateUpdate.c                                                      *
 * @author      Matt Ricci                                                         *
 * @addtogroup  RTOS			                                                         *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "FreeRTOS.h"
#include "semphr.h"
#include "message_buffer.h"
#include "event_groups.h"

#include "devicelist.h"

#include "dataframe.h"
#include "sensors.h"
#include "groups.h"

#include "accelerometer.h"
#include "state.h"
#include "membuff.h"
#include "stateupdate.h"

#include "AustralisConfig.h"

extern EventGroupHandle_t xTaskEnableGroup;
extern MessageBufferHandle_t xUsbTxBuff;
extern SemaphoreHandle_t xUsbMutex;

/* =============================================================================== */
/**
 * @brief State update task.
 *
 * Handles transitions between different flight states, sends CAN messages
 * for aerobrakes and altitude data, and enables or disables various data
 * acquisition tasks based on the current state.
 *
 * @todo Add definition for update period and replace assignments for frequency
 *       (e.g. xFrequency = pdMS_TO_TICKS(STATE_UPDATE_PERIOD);).
 **
 * =============================================================================== */
void vStateUpdate(void *argument) {
  const TickType_t xFrequency = pdMS_TO_TICKS(20); // 50Hz

  unsigned int CANHigh        = 0;
  unsigned int CANLow         = 0;
  unsigned int id             = 0;

  float avgPressCurrent       = 0;
  float avgPressPrevious      = 0;
  float avgVelCurrent         = 0;
  float avgVelPrevious        = 0;

  State *state                = State_getState();

  DeviceHandle_t accelHandle  = DeviceList_getDeviceHandle(DEVICE_ACCEL);
  Accel_t *accel              = accelHandle.device;

  for (;;) {
    // Block until 20ms interval
    TickType_t xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    switch (state->flightState) {
    case PRELAUNCH:
      if (accel->accelData[ZINDEX] >= ACCEL_LAUNCH) {
        xEventGroupSetBits(xTaskEnableGroup, GROUP_TASK_ENABLE_FLASH);   // Enable flash
        xEventGroupSetBits(xTaskEnableGroup, GROUP_TASK_ENABLE_HIGHRES); // Enable high resolution data acquisition
        xEventGroupSetBits(xTaskEnableGroup, GROUP_TASK_ENABLE_LOWRES);  // Enable low resolution data acquisition
        state->flightState = LAUNCH;
      }
      break;

    case LAUNCH:
      state->avgVel.calculateMovingAverage(&state->avgVel, &avgVelCurrent);
      // Transition to motor burnout state on velocity decrease
      if ((avgVelCurrent - avgVelPrevious) < 0) {
        state->flightState = COAST;
      }
      avgVelPrevious = avgVelCurrent;
      break;

    case COAST:
      state->avgPress.calculateMovingAverage(&state->avgPress, &avgPressCurrent);
      // Transition to apogee state on three way vote of altitude, velocity, and tilt
      // apogee is determined as two of three conditions evaluating true
      if ((((avgPressCurrent - avgPressPrevious) > 0) + (state->tilt >= 90) + (state->velocity < 0.0f)) >= 2) {
        state->flightState = APOGEE;

        union U {
          TickType_t ticks;
          uint8_t *bytes;
        };
        union U u;
        u.ticks = xTaskGetTickCount();

        // Log apogee event to flash
        state->mem.append(&state->mem, HEADER_EVENT_APOGEE);
        state->mem.appendBytes(&state->mem, u.bytes, sizeof(TickType_t));
        // Send transmission to trigger apogee E-matches
      }
      avgPressPrevious = avgPressCurrent;
      break;

    case APOGEE:
      // Deploy drogue chute
      //
      // Transition to descent state when below main deployment altitude
      if (state->altitude <= MAIN_ALTITUDE_METERS) {
        state->flightState = DESCENT;
        // Add descent event dataframe to buffer
      }
      break;

    case DESCENT:
      // Handle descent state actions
      break;
    }
  }
}

/** @} */
