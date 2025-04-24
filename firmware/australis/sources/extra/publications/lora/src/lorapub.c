/**************************************************************************************************
 * @file  loracomm.c                                                                              *
 * @brief Implements the FreeRTOS tasks and Interrupt Service Routine (ISR)                       *
 *        responsible for managing LoRa communication.                                            *
 *                                                                                                *
 * This file contains the implementation for LoRa transmit and receive tasks. It utilizes a       *
 * publication topic (`loraTopic`) to interface with other parts of the application.              *
 *                                                                                                *
 * The transmit task waits for messages on the topic's queue and transmits them via the LoRa      *
 * transceiver                                                                                    *
 *                                                                                                *
 * The receive task waits for notifications from the ISR (indicating a received packet), reads    *
 * the data from the transceiver, and publishes it to the topic.                                  *
 *                                                                                                *
 * The `EXTI1_IRQHandler` handles interrupts from the LoRa module (e.g., Tx Done, Rx Done),       *
 * notifying the appropriate task to proceed.                                                     *
 *                                                                                                *
 * @{                                                                                             *
 **************************************************************************************************/

#include "lorapub.h"

#include "stm32f439xx.h"

#include "FreeRTOS.h"
#include "event_groups.h"
#include "message_buffer.h"
#include "queue.h"

#include "_topic.h"
#include "devicelist.h"
#include "gpiopin.h"
#include "lora.h"

// NOTE:
// This topic is exposed for reader
// comments in the public header.
CREATE_TOPIC(lora, 10, LORA_MSG_LENGTH) // Create publication topic for GPS data
Topic *loraTopic = (Topic *)&lora;

TaskHandle_t vLoRaTransmitHandle;
TaskHandle_t vLoRaReceiveHandle;

static LoRa_t *transceiver;

/* ============================================================================================== */
/**
 * @brief LoRa transmit task.
 *
 * Handles transmission of data to the LoRa transceiver.
 *
 * This task blocks until a comment is received on the LoRa topic. Once received,
 * the data is transmitted and the task blocks until notified of transmit completion.
 **
 * ============================================================================================== */
void vLoRaTransmit(void *argument) {
  const TickType_t blockTime = portMAX_DELAY;
  uint8_t txData[LORA_MSG_LENGTH];

  vLoRaTransmitHandle = xTaskGetCurrentTaskHandle();
  GPIOpin_t rfToggle  = GPIOpin_init(GPIOE, GPIO_PIN2, NULL);

  // TODO:
  // Add deviceReady flag to driver API to indicate
  // when a device struct is initialised and populated
  //
  // Check if device pointer is NULL
  if (!transceiver)
    // Initialise transceiver if necessary
    transceiver = DeviceList_getDeviceHandle(DEVICE_LORA).device;

  for (;;) {
    // Wait to receive message to transmit
    BaseType_t result = xQueueReceive(
      lora.public.commentQueue, // Read from LoRa topic comment queue
      (void *)txData,           // Store data in binary array
      portMAX_DELAY             // Block forever until comment is available
    );

    // Transmit data if successfully retrieved from queue
    if (result == pdTRUE) {
      // Begin transmission
      rfToggle.reset(&rfToggle);                                   // Toggle RF front-end for transmit
      transceiver->transmit(transceiver, txData, LORA_MSG_LENGTH); // Send data to transmit

      // Wait for notification from ISR
      xTaskNotifyWait(0, 0, NULL, blockTime);

      // Return to receiving packets
      rfToggle.set(&rfToggle);                // Toggle RF front-end for receive
      transceiver->startReceive(transceiver); // Continue receiving
    }
  }
}

/* ============================================================================================== */
/**
 * @brief LoRa receive task.
 *
 * Handles receiving data from the LoRa transceiver.
 *
 * This task blocks until a notification is sent of a received packet. This can
 * only occur after being set to receive mode, either by the transmit task or
 * through a manual override in the driver.
 * Once notified, the packet data is read from the device and published to the
 * LoRa topic.
 **
 * ============================================================================================== */
void vLoRaReceive(void *argument) {
  const TickType_t blockTime = portMAX_DELAY;
  uint8_t rxData[LORA_MSG_LENGTH];

  vLoRaReceiveHandle = xTaskGetCurrentTaskHandle();
  GPIOpin_t rfToggle = GPIOpin_init(GPIOE, GPIO_PIN2, NULL);

  // Check if device pointer is NULL
  if (!transceiver)
    // Initialise transceiver if necessary
    transceiver = DeviceList_getDeviceHandle(DEVICE_LORA).device;

  for (;;) {
    // Wait for notification from ISR
    xTaskNotifyWait(0, 0, NULL, blockTime);

    // Read received packet from transceiver
    transceiver->readReceive(transceiver, rxData, LORA_MSG_LENGTH);

    // Publish packet data to topic
    Topic_publish((PrivateTopic *)loraTopic, rxData);
  }
}

/* ============================================================================================== */
/**
 * @brief LoRa Tx/Rx complete interrupt handler.
 *
 * Handles the external interrupt triggered by the Tx and Rx complete signals from
 * the LoRa transceiver. Upon interrupt, the appropriate task is notified according
 * to the current LoRa operating mode.
 **
 * ============================================================================================== */
void EXTI1_IRQHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult;

  // Clear pending interrupt
  EXTI->PR |= (0x02);

  // Clear transceiver IRQ and exit interrupt if
  // transceiver or tasks are not ready
  if (transceiver == NULL
      || vLoRaReceiveHandle == NULL
      || vLoRaTransmitHandle == NULL)
    goto LORA_NOT_READY;

  // Check if transceiver was transmitting
  if (transceiver->currentMode == LORA_MODE_TX) {
    // Send notification to transmit task and yield
    xTaskNotifyFromISR(vLoRaTransmitHandle, 0, eNoAction, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

  // Check if transceiver was receiving
  if (transceiver->currentMode == LORA_MODE_RX) {
    // Send notification to receive task and yield
    xTaskNotifyFromISR(vLoRaReceiveHandle, 0, eNoAction, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }

LORA_NOT_READY:
  // TODO: Implement specific clear methods for
  //       Tx and Rx IRQs for better abstraction
  transceiver->clearIRQ(transceiver, 0xFF);
}

/** @} */
