/**************************************************************************************************
 * @file  gpsacquisition.c                                                                        *
 * @brief Implements the FreeRTOS tasks and Interrupt Service Routine (ISR)                       *
 *        responsible for managing GPS communication.                                             *
 *                                                                                                *
 * This file contains the implementation for GPS acuisition. It utilizes a publication topic      *
 * (`gps`) to interface with other parts of the application.                                      *
 *                                                                                                *
 * The acuire task waits for notifications from the ISR (indicating a received packet), reads     *
 * the data from the receiver, and publishes it to the topic.                                     *
 *                                                                                                *
 *  TODO: Update this documentation comment                                                       *
 * The `EXTI1_IRQHandler` handles interrupts from the LoRa module (e.g., Tx Done, Rx Done),       *
 * notifying the appropriate task to proceed.                                                     *
 *                                                                                                *
 * TODO: Move hardcoded interrupt handler to function that can be called by / aliased to the      *
 * target specific interrupt handler.                                                             *
 * TODO: Replace hardcoded USART device in interrupt handler with device specific UART            *
 *                                                                                                *
 * @{                                                                                             *
 **************************************************************************************************/

#include "gpsacquisition.h"

#include "FreeRTOS.h"

#include "_topic.h"
#include "devicelist.h"

#include "sam_m10q.h"

// Create publication topic for GPS data
// NOTE:
// This topic is not exposed for reader
// comments in the public header.
CREATE_TOPIC(gps, 10, sizeof(SAM_M10Q_Data))

static TaskHandle_t vGpsAcquireHandle;
static SAM_M10Q_t *receiver;

#define GPS_RX_SIZE 128 // TODO: Refactor hardcoded value

static uint8_t gpsRxBuff[GPS_RX_SIZE];
static uint8_t gpsRxBuffIdx = 0;

/* ============================================================================================== */
/**
 * @brief
 *
 * @return .
 **
 * ============================================================================================== */
void vGpsAcquire(void *argument) {

  vGpsAcquireHandle = xTaskGetCurrentTaskHandle();

  // TODO:
  // Add deviceReady flag to driver API to indicate
  // when a device struct is initialised and populated
  //
  // Check if device pointer is NULL
  if (!receiver)
    // Initialise receiver if necessary
    receiver = DeviceList_getDeviceHandle(DEVICE_GPS).device;

  for (;;) {
    // Block until 500ms interval
    TickType_t xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));

    // Send GPS poll message
    receiver->pollPUBX(receiver);

    // Wait for notification from ISR
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

    // Parse NMEA data to struct
    SAM_M10Q_Data gpsData;
    receiver->parsePUBX(receiver, gpsRxBuff, &gpsData);

    // Publish parsed GPS data to topic
    Topic_publish(&gps, (uint8_t *)&gpsData);

    gpsRxBuffIdx = 0;
  }
}

/* ============================================================================================== */
/**
 * TODO: Replace IRQ handler with function to be called within a target defined handler
 *
 * @brief Interrupt handler for USB UART receive.
 *
 * This handler is triggered when data is received via USB UART. It appends the
 * received byte to a circular buffer and sends it to a stream buffer for
 * processing by the USB receive task.
 **
 * ============================================================================================== */
void USART3_IRQHandler() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Early exit interrupt if receiver or task is not ready
  if (receiver == NULL || vGpsAcquireHandle == NULL)
    return;

  // Read in byte from USART3
  while ((USART3->SR & USART_SR_RXNE) == 0);
  uint8_t rxData = USART3->DR & 0xFF;

  // Add byte to circular data buffer
  gpsRxBuff[gpsRxBuffIdx++]  = rxData;      // Add data and increment index
  gpsRxBuffIdx              %= GPS_RX_SIZE; // Wrap index on overflow

  // Send message to buffer on carriage return
  if (rxData == LINE_FEED) {
    xTaskNotifyFromISR(vGpsAcquireHandle, 0, eNoAction, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}
