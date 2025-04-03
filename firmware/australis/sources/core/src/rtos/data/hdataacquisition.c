/***********************************************************************************
 * @file        hDataAcquisition.c                                                 *
 * @author      Matt Ricci                                                         *
 * @addtogroup  RTOS			                                                         *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "FreeRTOS.h"
#include "event_groups.h"
#include "groups.h"
#include "message_buffer.h"
#include "semphr.h"

#include "dataframe.h"
#include "devicelist.h"
#include "membuff.h"
#include "sensors.h"

#include "state.h"
#include "accelerometer.h"
#include "gyroscope.h"
#include "hdataacquisition.h"

#include "quaternion.h"

#ifdef DUMMY
  #include "accelX.h"
  #include "accelY.h"
  #include "accelZ.h"
  #include "gyroX.h"
  #include "gyroY.h"
  #include "gyroZ.h"
#endif

extern long hDummyIdx;
char HdebugStr[100] = {};

extern EventGroupHandle_t xTaskEnableGroup;
extern MessageBufferHandle_t xUsbTxBuff;
extern SemaphoreHandle_t xUsbMutex;

/* =============================================================================== */
/**
 * @brief High-frequency data acquisition task.
 *
 * Acquires sensor data at a 500Hz rate. It selects the appropriate accelerometer
 * based on current data, processes sensor data and appends the processed data
 * to a dataframe. Optionally, dummy data can be used for testing if the `DUMMY`
 * macro is defined.
 *
 * Quaternion integration and tilt angle calculations are performed if enabled.
 *
 * @todo Add definition for sample period and replace assignments for dt and
 *       frequency (e.g. dt = 1/SAMPLE_PERIOD_HIGH;).
 **
 * =============================================================================== */
void vHDataAcquisition(void *argument) {
  float dt                    = 0.002;

  const TickType_t xFrequency = pdMS_TO_TICKS(2); // 500Hz
  const TickType_t blockTime  = pdMS_TO_TICKS(0);

  // Devices
  Accel_t *hAccel = DeviceList_getDeviceHandle(DEVICE_ACCEL_HIGH).device;
  Accel_t *lAccel = DeviceList_getDeviceHandle(DEVICE_ACCEL_LOW).device;
  Gyro_t *gyro    = DeviceList_getDeviceHandle(DEVICE_GYRO).device;

  // Selected accelerometer (high/low)
  DeviceHandle_t *accelHandlePtr = DeviceList_getDeviceHandlePointer(DEVICE_ACCEL);
  Accel_t *accel                 = accelHandlePtr->device;

  State *state                   = State_getState();

  for (;;) {
    // Block until 2ms interval
    TickType_t xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    // Select which accelerometer to use
    accelHandlePtr->device = (accel->accelData[ZINDEX] < 15) ? lAccel : hAccel;

    #ifdef DUMMY
      // Load bearing definition???
      const unsigned long accelX_length = 0x00007568;
      /*
      * Update sensor data with dummy values
      * These arrays are defined in the files under /Data and are generated from
      * past flight data binaries with srec_cat.
      */
      if (hDummyIdx < ACCELX_LENGTH - 1) {
        // Shift in floating point values and add to processed accelerometer array
        uint32_t tempX = (uint32_t)accelX[hDummyIdx + 1] << 16 | accelX[hDummyIdx];
        uint32_t tempY = (uint32_t)accelY[hDummyIdx + 1] << 16 | accelY[hDummyIdx];
        uint32_t tempZ = (uint32_t)accelZ[hDummyIdx + 1] << 16 | accelZ[hDummyIdx];
        memcpy(&accel->accelData[0], &tempX, sizeof(float));
        memcpy(&accel->accelData[1], &tempY, sizeof(float));
        memcpy(&accel->accelData[2], &tempZ, sizeof(float));

        // Back convert to raw data
        uint16_t xRaw          = (short)(accel->accelData[0] / accel->sensitivity);
        uint16_t yRaw          = (short)(accel->accelData[1] / accel->sensitivity);
        uint16_t zRaw          = (short)(accel->accelData[2] / accel->sensitivity);
        accel->rawAccelData[0] = xRaw >> 8;
        accel->rawAccelData[1] = xRaw;
        accel->rawAccelData[2] = yRaw >> 8;
        accel->rawAccelData[3] = yRaw;
        accel->rawAccelData[4] = zRaw >> 8;
        accel->rawAccelData[5] = zRaw;

        // Shift in floating point values and add to processed gyroscope array
        tempX = (uint32_t)gyroX[hDummyIdx + 1] << 16 | gyroX[hDummyIdx];
        tempY = (uint32_t)gyroY[hDummyIdx + 1] << 16 | gyroY[hDummyIdx];
        tempZ = (uint32_t)gyroZ[hDummyIdx + 1] << 16 | gyroZ[hDummyIdx];
        memcpy(&gyro->gyroData[0], &tempX, sizeof(float));
        memcpy(&gyro->gyroData[1], &tempY, sizeof(float));
        memcpy(&gyro->gyroData[2], &tempZ, sizeof(float));

        // Back convert to raw data
        xRaw                 = (short)(gyro->gyroData[0] / gyro->sensitivity);
        yRaw                 = (short)(gyro->gyroData[1] / gyro->sensitivity);
        zRaw                 = (short)(gyro->gyroData[2] / gyro->sensitivity);
        gyro->rawGyroData[0] = xRaw >> 8;
        gyro->rawGyroData[1] = xRaw;
        gyro->rawGyroData[2] = yRaw >> 8;
        gyro->rawGyroData[3] = yRaw;
        gyro->rawGyroData[4] = zRaw >> 8;
        gyro->rawGyroData[5] = zRaw;

        hDummyIdx += 2;
      }
    #else
      taskENTER_CRITICAL();
      lAccel->update(lAccel);
      hAccel->update(hAccel);
      gyro->update(gyro);
      taskEXIT_CRITICAL();
    #endif

    // Add sensor data to dataframe
    state->mem.append(&state->mem, HEADER_HIGHRES);
    state->mem.appendBytes(&state->mem, accel->rawAccelData, accel->dataSize);
    state->mem.appendBytes(&state->mem, gyro->rawGyroData, gyro->dataSize);

    // Only run calculations when enabled
    EventBits_t uxBits = xEventGroupWaitBits(xTaskEnableGroup, GROUP_TASK_ENABLE_HIGHRES, pdFALSE, pdFALSE, blockTime);
    if (uxBits & GROUP_TASK_ENABLE_HIGHRES) {
      // Integrate attitude quaternion from rotations
      Quaternion qDot = Quaternion_new();
      qDot.fromEuler(
          &qDot,
          (float)(dt * gyro->gyroData[ROLL_INDEX]),
          (float)(dt * gyro->gyroData[PITCH_INDEX]),
          (float)(dt * gyro->gyroData[YAW_INDEX])
      );
      state->rotation = Quaternion_mul(&state->rotation, &qDot);
      state->rotation.normalise(&state->rotation); // New attitude quaternion

      // Apply rotation to z-axis unit vector
      state->rotation.fRotateVector3D(&state->rotation, state->launchAngle, state->attitude);

      // Calculate tilt angle
      // tilt = cos^-1(attitude · initial)
      state->cosine = state->launchAngle[0] * state->attitude[0] + state->launchAngle[1] * state->attitude[1] + state->launchAngle[2] * state->attitude[2];
      state->tilt   = acos(state->cosine) * 180 / M_PI;
    }

    #ifdef DEBUG
      //! @todo extract debug print to function
      //! @todo move debug function to new source file with context as parameter
      if ((xSemaphoreTake(xUsbMutex, pdMS_TO_TICKS(0))) == pdTRUE) {
        memset(HdebugStr, 100, sizeof(char));

        snprintf(HdebugStr, 100, "[HDataAcq] %d\tAccel\tX: %.3f\tY: %.3f\tZ: %.3f\n\r", hDummyIdx / 2, accel->accelData[0], accel->accelData[1], accel->accelData[2]);
        xMessageBufferSend(xUsbTxBuff, (void *)HdebugStr, 100, pdMS_TO_TICKS(0));

        snprintf(HdebugStr, 100, "[HDataAcq] %d\tGyro\tX: %.3f\tY: %.3f\tZ: %.3f\n\r", hDummyIdx / 2, gyro->gyroData[0], gyro->gyroData[1], gyro->gyroData[2]);

        xMessageBufferSend(xUsbTxBuff, (void *)HdebugStr, 100, pdMS_TO_TICKS(0));
        xSemaphoreGive(xUsbMutex);
      }
    #endif
  }
}

/** @} */
