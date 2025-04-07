/***********************************************************************************
 * @file        tasks.c                                                            *
 * @author      Matt Ricci                                                         *
 * @brief                                                                          *
 ***********************************************************************************/

#include "FreeRTOS.h"
#include "task.h"

#include "stdbool.h"

#include "gpiopin.h"
#include "tasklist.h"

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
  xTaskCreate(vHeartbeatBlink, "HeartbeatBlink", 128, NULL, tskIDLE_PRIORITY + 1, TaskList_new());
  return true;
}
