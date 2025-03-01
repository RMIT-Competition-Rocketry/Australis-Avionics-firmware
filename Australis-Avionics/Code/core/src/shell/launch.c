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

#include "kx134_1211.h"

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
  DeviceHandle_t accelHandle = DeviceList_getDeviceHandle(DEVICE_ACCEL);
  KX134_1211_t *accel        = accelHandle.device;
  accel->accelData[ZINDEX]   = ACCEL_LAUNCH;
  TaskHandle_t handle        = xTaskGetHandle("StateUpdate");
  xTaskAbortDelay(handle);
}

/** @} */
