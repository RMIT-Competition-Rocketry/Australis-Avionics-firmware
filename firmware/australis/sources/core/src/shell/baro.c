/***********************************************************************************
 * @file        launch.c                                                           *
 * @author      Matt Ricci                                                         *
 * @addtogroup  Shell                   																					 *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "stdint.h"
#include "stdio.h"

#include "shell.h"
#include "devicelist.h"

#include "barometer.h"

static void Baro_exec(Shell *shell, uint8_t *);

DEFINE_PROGRAM_HANDLE("baro", Baro_exec)

/* =============================================================================== */
/**
 * @brief
 *
 *
 **
 * =============================================================================== */
static void Baro_exec(Shell *shell, uint8_t *flags) {
  Baro_t *baro = DeviceList_getDeviceHandle(DEVICE_BARO).device;
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
