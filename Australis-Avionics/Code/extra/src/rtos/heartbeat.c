/***********************************************************************************
 * @file        heartbeat.c                                                        *
 * @author      Matt Ricci                                                         *
 * @addtogroup  RTOS			                                                         *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "FreeRTOS.h"
#include "task.h"

#include "heartbeat.h"

#include "gpiopin.h"
#include "devices.h"

/* =============================================================================== */
/**
 * @brief
 **
 * =============================================================================== */
void vHeartbeatBlink(void *argument) {

  const TickType_t xFrequency = pdMS_TO_TICKS(125);
  GPIOpin_t heartbeatLED      = GPIOpin_init(LED1_PORT, LED1_PIN, NULL);

  for (;;) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    heartbeatLED.toggle(&heartbeatLED);
  }
}

/** @} */
