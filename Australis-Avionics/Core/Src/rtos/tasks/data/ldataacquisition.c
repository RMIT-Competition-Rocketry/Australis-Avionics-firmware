/***********************************************************************************
 * @file        lDataAcquisition.c                                                 *
 * @author      Matt Ricci                                                         *
 * @addtogroup  RTOS			                                                         *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "ldataacquisition.h"
#include "math.h"
#include "uart.h"

extern EventGroupHandle_t xTaskEnableGroup;
extern SemaphoreHandle_t xUsbMutex;
extern MessageBufferHandle_t xUsbTxBuff;
extern long lDummyIdx;
char LdebugStr[100] = {};

/* =============================================================================== */
/**
 * @brief Low-frequency data acquisition and altitude estimation function.
 *
 * Performs data acquisition for barometric pressure at a 50Hz rate.
 * Altitude is calculated from barometric pressure using the hypsometric formula
 * Kalman filter state matrices (A, Q, R, P) are initialized within the function.
 * Optionally, dummy data can be used for testing if the `DUMMY` macro is defined.
 *
 * Velocity and altitude state estimates are calculated with the Kalman filter
 * if enabled.
 *
 * @todo Add definition for sample period and replace assignments for dt and
 *       frequency (e.g. dt = 1/SAMPLE_PERIOD_LOW;).
 **
 * =============================================================================== */
void vLDataAcquisition(void *argument) {
  float dt = 0.020;
  KalmanFilter kf;
  KalmanFilter_init(&kf);

  //! @todo Move kalman filter matrices into context struct
  // Initialise filter parameters
  float A[9] = {
      1.0, dt, 0.5 * (dt * dt),
      0.0, 1.0, dt,
      0.0, 0.0, 1.0
  };
  kf.A.pData = A;
  float Q[9] = {
      99.52, 0.0, 0.0,
      0.0, 1.42, 0.0,
      0.0, 0.0, 6.27
  };
  kf.Q.pData = Q;
  float R[4] = {
      97.92, 0.0,
      0.0, 0.61
  };
  kf.R.pData = R;
  float P[9] = {
      1, 0.0, 0.0,
      0.0, 0.1, 0.0,
      0.0, 0.0, 100.0
  };
  kf.P.pData = P;

  // Initialise measurement matrix
  arm_matrix_instance_f32 z;
  float zData[2] = {0.0, 0.0};
  arm_mat_init_f32(&z, 2, 1, zData);

  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(20); // 50Hz
  const TickType_t blockTime  = pdMS_TO_TICKS(0);

  MemBuff *mem                = (MemBuff *)argument;
  BMP581 *baro                = DeviceHandle_getHandle("Baro").device;

  UART *usb                   = DeviceHandle_getHandle("USB").device;
  DeviceHandle_t accelHandle  = DeviceHandle_getHandle("Accel");
  KX134_1211 *accel           = accelHandle.device;

  float *altitude             = StateHandle_getHandle("Altitude").state;
  float *cosine               = StateHandle_getHandle("Cosine").state;
  float *velocity             = StateHandle_getHandle("Velocity").state;
  SlidingWindow *avgVel       = StateHandle_getHandle("AvgVelBuffer").state;
  SlidingWindow *avgPress     = StateHandle_getHandle("AvgPressBuffer").state;

  for (;;) {
    // Block until 20ms interval
    TickType_t xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    // Update baro data
#ifdef DUMMY
    const unsigned long press_length = 0x00003A5C;
    if (lDummyIdx < PRESS_LENGTH - 1) {
      uint32_t tempPress = (uint32_t)press[lDummyIdx + 1] << 16 | press[lDummyIdx];
      memcpy(&baro->press, &tempPress, sizeof(float));
      lDummyIdx += 2;
    }
#else
    taskENTER_CRITICAL();
    baro->update(baro);
    taskEXIT_CRITICAL();
#endif

    // Calculate altitude
    *altitude = 44330 * (1.0 - pow(baro->press / baro->groundPress, 0.1903));

    // Add sensor data and barometer data to dataframe
    mem->append(mem, HEADER_LOWRES);
    mem->appendBytes(mem, baro->rawTemp, BMP581_DATA_SIZE);
    mem->appendBytes(mem, baro->rawPress, BMP581_DATA_SIZE);

    // Only run calculations when enabled
    EventBits_t uxBits = xEventGroupWaitBits(xTaskEnableGroup, GROUP_TASK_ENABLE_LOWRES, pdFALSE, pdFALSE, blockTime);
    if (uxBits & GROUP_TASK_ENABLE_LOWRES) {
      // Calculate state
      z.pData[0] = *altitude;
      z.pData[1] = (*cosine * 9.81 * accel->accelData[ZINDEX] - 9.81); // Acceleration measured in m/s^2
      kf.update(&kf, &z);

      *velocity = kf.x.pData[1];
      avgPress->append(avgPress, baro->press);
      avgVel->append(avgVel, *velocity);
    }

#ifdef DEBUG
    //! @todo extract debug print to function
    //! @todo move debug function to new source file with context as parameter
    if ((xSemaphoreTake(xUsbMutex, pdMS_TO_TICKS(0))) == pdTRUE) {
      char debugStr[100];
      snprintf(debugStr, 100, "[LDataAcq] %d\tBaro\tPressure: %.0f\tTemperature: %.1f\n\r", lDummyIdx / 2, baro->press, baro->temp);
      xMessageBufferSend(xUsbTxBuff, (void *)debugStr, 100, pdMS_TO_TICKS(10));
      xSemaphoreGive(xUsbMutex);
    }
#endif
  }
}

/** @} */
