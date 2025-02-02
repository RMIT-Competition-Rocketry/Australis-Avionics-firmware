/***********************************************************************************
 * @file        launch.c                                                           *
 * @author      Matt Ricci                                                         *
 * @addtogroup  Shell                   																					 *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/
 
#include "mem.h"

static void Flash_exec(Shell *, uint8_t *);
	
static ShellProgramHandle_t registerShellProgram() {
	return (ShellProgramHandle_t){
		.name = "flash",
		.exec = Flash_exec
	};
}

__attribute__((section(".shell_flash"), unused))
static ShellProgramHandle_t (*registerShellProgram_ptr)() = registerShellProgram;

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
	Flash *flash = DeviceHandle_getHandle("Flash").device;
	UART *usb = DeviceHandle_getHandle("USB").device;
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