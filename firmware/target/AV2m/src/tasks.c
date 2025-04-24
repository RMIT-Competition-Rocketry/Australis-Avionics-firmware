/***********************************************************************************
 * @file        tasks.c                                                            *
 * @author      Matt Ricci                                                         *
 * @brief                                                                          *
 ***********************************************************************************/

#include "FreeRTOS.h"
#include "task.h"

#include "stdbool.h"

#include "shell.h"
#include "tasklist.h"
#include "devices.h"

#include "usbcomm.h"
#include "lorapub.h"
#include "gpsacquisition.h"
#include "groundcomms.h"

void vHeartbeatBlink(void *argument) {

  const TickType_t xFrequency = pdMS_TO_TICKS(675);
  GPIOpin_t heartbeatLED      = GPIOpin_init(LED1_PORT, LED1_PIN, NULL);

  heartbeatLED.reset(&heartbeatLED);

  for (;;) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    heartbeatLED.toggle(&heartbeatLED);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(130));
    heartbeatLED.toggle(&heartbeatLED);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(130));
    heartbeatLED.toggle(&heartbeatLED);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(125));
    heartbeatLED.toggle(&heartbeatLED);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(125));
  }
}

/**
 * @todo Refactor and document
 */
void vEnableInterrupts() {
  __disable_irq();
  NVIC_SetPriority(EXTI1_IRQn, 9);
  NVIC_EnableIRQ(EXTI1_IRQn);
  NVIC_SetPriority(USART6_IRQn, 10);
  NVIC_EnableIRQ(USART6_IRQn);
  NVIC_SetPriority(USART3_IRQn, 11);
  NVIC_EnableIRQ(USART3_IRQn);
  EXTI->RTSR        |= 0x02;
  EXTI->IMR         |= 0x02;
  SYSCFG->EXTICR[0] &= ~0xF0;
  SYSCFG->EXTICR[0]  = 0x30;
  __enable_irq();

  vTaskDelete(NULL);
}

/* ============================================================================================== */
/**
 * @brief Initialise and store FreeRTOS task handles not handled by the Australis core.
 *
 * @return .
 **
 * ============================================================================================== */

bool initTasks() {

  // Initialise shell
  static Shell shell;
  Shell_init(&shell);

  xTaskCreate(vHeartbeatBlink, "HeartbeatBlink", 128, NULL, configMAX_PRIORITIES - 1, TaskList_new());

  xTaskCreate(vGroundCommStateMachine, "GroundComms", 512, NULL, configMAX_PRIORITIES - 5, TaskList_new());
  xTaskCreate(vLoRaTransmit, "LoraTx", 256, NULL, configMAX_PRIORITIES - 5, TaskList_new());
  xTaskCreate(vLoRaReceive, "LoraRx", 256, NULL, configMAX_PRIORITIES - 5, TaskList_new());
  xTaskCreate(vGpsAcquire, "GpsRx", 256, NULL, configMAX_PRIORITIES - 5, TaskList_new());
  xTaskCreate(vUsbTransmit, "UsbTx", 256, NULL, configMAX_PRIORITIES - 6, TaskList_new());
  xTaskCreate(vUsbReceive, "UsbRx", 256, &shell, configMAX_PRIORITIES - 6, TaskList_new());

  xTaskCreate(vEnableInterrupts, "interrupts", 128, NULL, tskIDLE_PRIORITY, TaskList_new());
  return true;
}
