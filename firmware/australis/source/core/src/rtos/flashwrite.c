/***********************************************************************************
 * @file        flashWrite.c                                                       *
 * @author      Matt Ricci                                                         *
 * @addtogroup  RTOS			                                                         *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "FreeRTOS.h"
#include "event_groups.h"
#include "stdbool.h"

#include "groups.h"

#include "devicelist.h"
#include "w25q128.h"
#include "membuff.h"

#include "state.h"
#include "flashwrite.h"

extern EventGroupHandle_t xTaskEnableGroup;

/* =============================================================================== */
/**
 * @brief Idle hook for writing to flash.
 *
 * Uses idle time to check if there is a page available in the buffer. If a page
 * is available and the rocket state is past `LAUNCH`, it sets the flash write
 * flag to trigger the flash buffer task.
 **
 * =============================================================================== */
void vIdle(void *argument) {

  State *state = State_getState();

  for (;;) {
    // Write if a page is available in the buffer
    if (state->flightState >= LAUNCH && state->mem.pageReady)
      xEventGroupSetBits(xTaskEnableGroup, GROUP_TASK_ENABLE_FLASH);
  }
}

/* =============================================================================== */
/**
 * @brief Flash buffer write task.
 *
 * Monitors flash enable flag to determine when to flush data from the memory buffer to
 * flash memory. The task waits for the flag to be set, reads data into a buffer, writes
 * the data to flash, and then updates the page address.
 **
 * =============================================================================== */
void vFlashBuffer(void *argument) {
  const TickType_t timeout = portMAX_DELAY;
  uint32_t pageAddr        = 0;

  State *state             = State_getState();

  W25Q128_t *flash         = DeviceList_getDeviceHandle(DEVICE_FLASH).device;
  uint8_t outBuff[flash->pageSize];

  for (;;) {
    // Wait for write flag to be ready, clear flag on exit
    EventBits_t uxBits = xEventGroupWaitBits(xTaskEnableGroup, GROUP_TASK_ENABLE_FLASH, pdTRUE, pdFALSE, timeout);
    if (uxBits & GROUP_TASK_ENABLE_FLASH) {
      // Flush data to output buffer
      bool success = state->mem.readPage(&state->mem, outBuff);
      if (success) {
        taskENTER_CRITICAL();
        // Write data to flash memory if within bounds
        if (pageAddr / flash->pageSize < flash->pageCount - 1)
          flash->writePage(flash, pageAddr, outBuff);

        // Increment page address
        pageAddr += 0x100;
        taskEXIT_CRITICAL();
      }
    }
  }
}

/** @} */
