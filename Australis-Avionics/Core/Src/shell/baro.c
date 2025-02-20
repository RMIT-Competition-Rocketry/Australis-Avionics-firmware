/***********************************************************************************
 * @file        launch.c                                                           *
 * @author      Matt Ricci                                                         *
 * @addtogroup  Shell                   																					 *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "baro.h"

static void Baro_exec(Shell *shell, uint8_t *);

static ShellProgramHandle_t registerShellProgram() {
  return (ShellProgramHandle_t){
      .name = "baro",
      .exec = Baro_exec
  };
}

__attribute__((section(".shell_baro"), unused)) static ShellProgramHandle_t (*registerShellProgram_ptr)() = registerShellProgram;

/* =============================================================================== */
/**
 * @brief
 *
 *
 **
 * =============================================================================== */
static void Baro_exec(Shell *shell, uint8_t *flags) {
  BMP581_t *baro = DeviceList_getDeviceHandle(DEVICE_BARO).device;
  char str[30];
  if (!strcmp(flags, "read")) {
    baro->update(baro);
    snprintf(str, 50, "Ground pressure: %f\n\r", baro->groundPress);
    shell->usb.print(&shell->usb, str);
    snprintf(str, 50, "Current pressure: %f\n\r", baro->press);
    shell->usb.print(&shell->usb, str);
    snprintf(str, 50, "Current temperature: %f\n\r", baro->temp);
    shell->usb.print(&shell->usb, str);
  }
}

/** @} */
