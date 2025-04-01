/***********************************************************************************
 * @file        launch.c                                                           *
 * @author      Matt Ricci                                                         *
 * @addtogroup  Shell                   																					 *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "devicelist.h"
#include "shell.h"
#include "params.h"
#include "sensors.h"

#include "accelerometer.h"

static void Launch_exec(Shell *, uint8_t *);

DEFINE_PROGRAM_HANDLE("launch", Launch_exec)

/* =============================================================================== */
/**
 * @brief
 *
 *
 **
 * =============================================================================== */
static void Launch_exec(Shell *shell, uint8_t *flags) {
  Accel_t *accel           = DeviceList_getDeviceHandle(DEVICE_ACCEL).device;
  accel->accelData[ZINDEX] = ACCEL_LAUNCH;
  TaskHandle_t handle      = xTaskGetHandle("StateUpdate");
  xTaskAbortDelay(handle);
}

/** @} */
