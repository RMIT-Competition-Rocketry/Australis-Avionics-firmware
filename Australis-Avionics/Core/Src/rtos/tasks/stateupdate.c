/***********************************************************************************
 * @file        stateUpdate.c                                                      *
 * @author      Matt Ricci                                                         *
 * @addtogroup  RTOS			                                                         *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "stateupdate.h"
#include "drivers.h"
#include "membuff.h"
#include "stdio.h"

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

  Handles *handles            = (Handles *)argument;

  DeviceHandle_t accelHandle  = DeviceHandle_getHandle("Accel");
  KX134_1211 *accel           = accelHandle.device;

  MemBuff *mem                = StateHandle_getHandle("Memory").state;
  enum State *flightState     = StateHandle_getHandle("FlightState").state;
  float *tilt                 = StateHandle_getHandle("Tilt").state;
  float *altitude             = StateHandle_getHandle("Altitude").state;
  float *velocity             = StateHandle_getHandle("Velocity").state;
  SlidingWindow *avgVel       = StateHandle_getHandle("AvgVelBuffer").state;
  SlidingWindow *avgPress     = StateHandle_getHandle("AvgPressBuffer").state;

  for (;;) {
    // Block until 20ms interval
    TickType_t xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    // Emergency aerobrakes for excessive tilt
    if (*tilt >= 30.0f) {
      // CAN payload for aerobrakes retract
      CANHigh = 0x00000000;
      CANLow  = 0x00000000;
      id      = CAN_HEADER_AEROBRAKES_RETRACT;
      CAN_TX(CAN_AB, 8, CANHigh, CANLow, id);
    }

    switch (*flightState) {
    case PRELAUNCH:
      if (accel->accelData[ZINDEX] >= ACCEL_LAUNCH) {
#ifdef FLIGHT_TEST
        GPIOB->ODR ^= 0x8000;
        GPIOD->ODR ^= 0x8000;
#endif
#ifndef DEBUG
        //					vTaskSuspend(handles->xUsbReceiveHandle);
        //          vTaskDelete(handles->xUsbTransmitHandle);
        //          vTaskDelete(handles->xUsbReceiveHandle);
#endif
        xEventGroupSetBits(xTaskEnableGroup, GROUP_TASK_ENABLE_FLASH);   // Enable flash
        xEventGroupSetBits(xTaskEnableGroup, GROUP_TASK_ENABLE_HIGHRES); // Enable high resolution data acquisition
        xEventGroupSetBits(xTaskEnableGroup, GROUP_TASK_ENABLE_LOWRES);  // Enable low resolution data acquisition
        *flightState = LAUNCH;
      }
      break;

    case LAUNCH:
      avgVel->calculateMovingAverage(avgVel, &avgVelCurrent);
      // Send altitude to aerobrakes via CAN
      CANHigh = 0x00000000;
      memcpy(&CANLow, altitude, sizeof(float));
      id = CAN_HEADER_AEROBRAKES_DATA;
      CAN_TX(CAN_AB, 8, CANHigh, CANLow, id);
      // Transition to motor burnout state on velocity decrease
      if ((avgVelCurrent - avgVelPrevious) < 0) {
#ifdef FLIGHT_TEST
        GPIOB->ODR ^= 0x8000;
        GPIOD->ODR ^= 0x8000;
#endif
        *flightState = COAST;
      }
      avgVelPrevious = avgVelCurrent;
      break;

    case COAST:
      avgPress->calculateMovingAverage(avgPress, &avgPressCurrent);
      // Send altitude to aerobrakes via CAN
      CANHigh = 0x00000000;
      memcpy(&CANLow, altitude, sizeof(float));
      id = CAN_HEADER_AEROBRAKES_DATA;
      CAN_TX(CAN_AB, 8, CANHigh, CANLow, id);
      // Transition to apogee state on three way vote of altitude, velocity, and tilt
      // apogee is determined as two of three conditions evaluating true
      if ((((avgPressCurrent - avgPressPrevious) > 0) + (*tilt >= 90) + (*velocity < 0.0f)) >= 2) {
#ifdef FLIGHT_TEST
        GPIOB->ODR ^= 0x8000;
        GPIOD->ODR ^= 0x8000;
#endif
        *flightState = APOGEE;

        taskENTER_CRITICAL();
        buzzer(3215 * 5);
        taskEXIT_CRITICAL();

        union U {
          TickType_t ticks;
          uint8_t *bytes;
        };
        union U u;
        u.ticks = xTaskGetTickCount();

        // Log apogee event to flash
        mem->append(mem, HEADER_EVENT_APOGEE);
        mem->appendBytes(mem, u.bytes, sizeof(TickType_t));
        // Send transmission to trigger apogee E-matches
      }
      avgPressPrevious = avgPressCurrent;
      break;

    case APOGEE:
      // Retract aerobrakes
      CANHigh = 0x00000000;
      CANLow  = 0x00000000;
      id      = CAN_HEADER_AEROBRAKES_RETRACT;
      CAN_TX(CAN_AB, 8, CANHigh, CANLow, id);
      // Deploy drogue chute
      GPIOD->ODR |= 0x8000;
      // Transition to descent state when below main deployment altitude
      if (*altitude <= MAIN_ALTITUDE_METERS) {
#ifdef FLIGHT_TEST
        GPIOB->ODR ^= 0x8000;
        GPIOD->ODR ^= 0x8000;
#endif
        *flightState = DESCENT;
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
