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

#include "usbcomm.h"
#include "loracomm.h"
#include "groundcomms.h"

#define LED1_PIN  1
#define LED1_PORT GPIOA

void vHeartbeatBlink(void *argument) {

  const TickType_t xFrequency = pdMS_TO_TICKS(125);
  GPIOpin_t heartbeatLED      = GPIOpin_init(LED1_PORT, LED1_PIN, NULL);

  for (;;) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    heartbeatLED.toggle(&heartbeatLED);
  }
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
  xTaskCreate(vLoRaTransmit, "LoRaTx", 256, NULL, configMAX_PRIORITIES - 5, TaskList_new());
  xTaskCreate(vLoRaReceive, "LoRaRx", 256, NULL, configMAX_PRIORITIES - 5, TaskList_new());
  xTaskCreate(vUsbTransmit, "UsbTx", 256, NULL, configMAX_PRIORITIES - 6, TaskList_new());
  xTaskCreate(vUsbReceive, "UsbRx", 256, &shell, configMAX_PRIORITIES - 6, TaskList_new());
  return true;
}
