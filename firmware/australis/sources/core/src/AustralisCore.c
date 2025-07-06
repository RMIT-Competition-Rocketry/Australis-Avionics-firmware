/***********************************************************************************
 * @file        AustralisCore.c                                                    *
 * @author      Matt Ricci                                                         *
 * @brief       Main application entry point and system initialization.            *
 ***********************************************************************************/

#include "AustralisCore.h"
#include "stm32f439xx.h"

// -- STD Lib -----------------------------------------------

#include "stdio.h"

// -- FreeRTOS ----------------------------------------------

#include "FreeRTOS.h"
#include "event_groups.h"
#include "groups.h"
#include "message_buffer.h"
#include "semphr.h"
#include "stream_buffer.h"

// -- Australis Core ----------------------------------------

#include "flashwrite.h"
#include "hdataacquisition.h"
#include "ldataacquisition.h"
#include "state.h"
#include "stateupdate.h"
#include "tasklist.h"

// ------------------------------------------------------------

static void configure_interrupts();

// RTOS event groups
EventGroupHandle_t xTaskEnableGroup;   // 0: FLASH,  1: HIGHRES, 2: LOWRES, 3: LORA, 7: IDLE
EventGroupHandle_t xSystemStatusGroup; // 0-2: Flight state, 3: Payload, 4: Aerobrakes
EventGroupHandle_t xMsgReadyGroup;     // 0: LORA, 1: USB

// RTOS message buffers
MessageBufferHandle_t xUsbTxBuff;
StreamBufferHandle_t xUsbRxBuff;
StreamBufferHandle_t xGpsRxBuff;

SemaphoreHandle_t xUsbMutex;

#define AVG_BUFF_SIZE 15
#define MEM_BUFF_SIZE 20992
#define USB_TX_SIZE   25
#define USB_RX_SIZE   25

/* =============================================================================== */
/**
 * @brief Run the Australis core

 * =============================================================================== */

void Australis_startCore(void) {

  // Start the scheduler
  vTaskStartScheduler();
}

/* =============================================================================== */
/**
 * @brief Initialisation of RTOS tasks
 *
 * Performs initial setup for various peripherals, ensuring all components
 * are ready for data acquisition and system state management. This task also
 * initializes RTOS event groups and message buffers to manage inter-task
 * communication.
 *
 * @todo Refactor context parameters that require write operations to intialise as
 *       pointers within their respective structs.
 *
 * @return void
 * =============================================================================== */

void Australis_init() {

  #if coreUSE_TRACE == 1
    xTraceEnable(TRC_START);
  #endif

  // TODO: Replace usage of event groups for xTaskEnableGroup and xMsgReadyGroup
  //       with direct-to-task notifications, for better efficiency and clarity as
  //       these flags aren't shared across multiple tasks.

  // Initialise event groups for task synchronization and message signaling
  xTaskEnableGroup   = xEventGroupCreate(); // 0: FLASH,  1: HIGHRES, 2: LOWRES, 3: LORA, 7: IDLE
  xSystemStatusGroup = xEventGroupCreate();
  xMsgReadyGroup     = xEventGroupCreate();
  xEventGroupSetBits(xMsgReadyGroup, GROUP_MESSAGE_READY_LORA);

// Initialise USB buffers and mutex
// ALLOW FORMATTING
#ifdef USB_TX_SIZE
  xUsbTxBuff = xMessageBufferCreate(USB_TX_SIZE);
  xUsbRxBuff = xStreamBufferCreate(USB_RX_SIZE, 1);
#endif

  xUsbMutex = xSemaphoreCreateMutex();

  // Initialise core state
  State_init();

  /**********************************************************************************
   *                                    TASK INIT                                   *
   **********************************************************************************/

  #if (coreTASK_ENABLE == 1)
    xTaskCreate(vHDataAcquisition, "HDataAcq", 512, NULL, configMAX_PRIORITIES - 2, TaskList_new());
    xTaskCreate(vLDataAcquisition, "LDataAcq", 512, NULL, configMAX_PRIORITIES - 3, TaskList_new());
    xTaskCreate(vStateUpdate, "StateUpdate", 512, NULL, configMAX_PRIORITIES - 4, TaskList_new());
    xTaskCreate(vFlashBuffer, "FlashData", 512, NULL, configMAX_PRIORITIES - 1, TaskList_new());
    //xTaskCreate(vIdle, "Idle", 256, NULL, tskIDLE_PRIORITY, TaskList_new());
  #endif
}
