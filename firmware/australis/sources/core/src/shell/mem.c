/***********************************************************************************
 * @file        launch.c                                                           *
 * @author      Matt Ricci                                                         *
 * @addtogroup  Shell                   																					 *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "mem.h"

#include "devicelist.h"
#include "flash.h"
#include "shell.h"
#include "uart.h"

static void Flash_exec(Shell *, uint8_t *);

DEFINE_PROGRAM_HANDLE("flash", Flash_exec)

/* =============================================================================== */
/**
 * @brief Execute flash commands on target
 *
 * @details `usbFlashCommandExecute` parses and executes flash related commands
 * according to the flags passed in by the top level command parser.
 * @details Currently implemented commands include:
 * 	- Erase
 * 	- Read all
 **
 * =============================================================================== */
static void Flash_exec(Shell *shell, uint8_t *flags) {
  Flash_t *flash = DeviceList_getDeviceHandle(DEVICE_FLASH).device;
  UART_t *usb    = DeviceList_getDeviceHandle(DEVICE_UART_USB).device;
  // flash erase
  if (!strcmp(flags, CMD_FLASH_ERASE)) {
    usb->print(usb, "Clearing flash... ");
    flash->erase(flash);
    usb->print(usb, "Done.\n\r");
  }
  // flash read all
  else if (!strcmp(flags, CMD_FLASH_READ_ALL)) {
    vTaskSuspendAll();
    volatile uint8_t pageData[256];
    for (long i = 0; i < flash->pageCount; i++) {
      flash->readPage(flash, i * 0x100, pageData);
      for (int j = 0; j < flash->pageSize; j++)
        usb->send(usb, pageData[j]);
    }
    xTaskResumeAll();
  }
}

/** @} */
