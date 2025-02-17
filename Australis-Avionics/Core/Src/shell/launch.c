/***********************************************************************************
 * @file        launch.c                                                           *
 * @author      Matt Ricci                                                         *
 * @addtogroup  Shell                   																					 *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "devicelist.h"
#include "launch.h"

static void Launch_exec(Shell *, uint8_t *);

static ShellProgramHandle_t registerShellProgram() {
  return (ShellProgramHandle_t){
      .name = "launch",
      .exec = Launch_exec
  };
}

__attribute__((section(".shell_launch"), unused)) static ShellProgramHandle_t (*registerShellProgram_ptr)() = registerShellProgram;

/* =============================================================================== */
/**
 * @brief
 *
 *
 **
 * =============================================================================== */
static void Launch_exec(Shell *shell, uint8_t *flags) {
  DeviceHandle_t accelHandle = DeviceList_getDeviceHandle(DEVICE_ACCEL);
  KX134_1211 *accel          = accelHandle.device;
  accel->accelData[ZINDEX]   = ACCEL_LAUNCH;
  TaskHandle_t handle        = xTaskGetHandle("StateUpdate");
  xTaskAbortDelay(handle);
}

/** @} */
