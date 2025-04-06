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
MessageBufferHandle_t xLoRaTxBuff;
MessageBufferHandle_t xUsbTxBuff;
StreamBufferHandle_t xUsbRxBuff;
StreamBufferHandle_t xGpsRxBuff;

SemaphoreHandle_t xUsbMutex;

#define AVG_BUFF_SIZE  15
#define LORA_BUFF_SIZE 128
#define MEM_BUFF_SIZE  20992
#define USB_TX_SIZE    25
#define USB_RX_SIZE    25

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
  #ifdef USB_TX_SIZE
  xUsbTxBuff = xMessageBufferCreate(USB_TX_SIZE);
  xUsbRxBuff = xStreamBufferCreate(USB_RX_SIZE, 1);
  //xGpsRxBuff = xStreamBufferCreate(GPS_RX_SIZE, 1);
  #endif

  xUsbMutex = xSemaphoreCreateMutex();

  // Initialise LoRa buffer
  xLoRaTxBuff = xMessageBufferCreate(LORA_BUFF_SIZE);

  // TODO: As with the RCC, extract interrupt configuration to hardware specific
  //       target files in Target/ subdirectories. As some of the IRQ handlers will
  //       need to remain defined in RTOS application source files, the exact names
  //       of the defined functions may be formatted via preprocessor macro.

  // Enable peripheral and external interrupts
  // configure_interrupts();

  // Initialise core state
  State_init();

  /**********************************************************************************
   *                                    TASK INIT                                   *
   **********************************************************************************/

  xTaskCreate(vHDataAcquisition, "HDataAcq", 512, NULL, configMAX_PRIORITIES - 2, TaskList_new());
  xTaskCreate(vLDataAcquisition, "LDataAcq", 512, NULL, configMAX_PRIORITIES - 3, TaskList_new());
  xTaskCreate(vStateUpdate, "StateUpdate", 512, NULL, configMAX_PRIORITIES - 4, TaskList_new());
  xTaskCreate(vFlashBuffer, "FlashData", 512, NULL, configMAX_PRIORITIES - 1, TaskList_new());
  xTaskCreate(vIdle, "Idle", 256, NULL, tskIDLE_PRIORITY, TaskList_new());

  xTaskCreate(configure_interrupts, "interrupts", 128, NULL, tskIDLE_PRIORITY, TaskList_new());

  // TODO: Temporarily disabled due to bug related to use of message buffer.
  //       See gpsacquisition.c todo for more detail.

  // xTaskCreate(vGpsTransmit, "GpsRead", 512, NULL, configMAX_PRIORITIES - 6, &handles.xGpsTransmitHandle);
}

/**
 * @todo Refactor and document
 */
void configure_interrupts() {
  __disable_irq();
  NVIC_SetPriority(EXTI1_IRQn, 9);
  NVIC_EnableIRQ(EXTI1_IRQn);
  NVIC_SetPriority(USART6_IRQn, 10);
  NVIC_EnableIRQ(USART6_IRQn);
  NVIC_SetPriority(USART3_IRQn, 10);
  NVIC_EnableIRQ(USART3_IRQn);
  EXTI->RTSR        |= 0x02;
  EXTI->IMR         |= 0x02;
  SYSCFG->EXTICR[0] &= ~0xF0;
  SYSCFG->EXTICR[0]  = 0x30;
  __enable_irq();

  vTaskDelete(NULL);
}
