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

#include "AustralisConfig.h"
#include "lorapub.h"

#include "stm32f439xx.h"

#include "FreeRTOS.h"
#include "event_groups.h"
#include "message_buffer.h"
#include "queue.h"

#include "_topic.h"
#include "gpiopin.h"
#include "lora.h"

// Create publication topic for LoRa data
//
// NOTE:
// This topic is exposed for reader
// comments in the public header.
CREATE_TOPIC(lora, 200, LORA_MSG_LENGTH)
Topic *loraTopic = (Topic *)&lora;

static TaskHandle_t vLoRaTransmitHandle;
static TaskHandle_t vLoRaReceiveHandle;

// LoRa transceiver device
//
// TODO:
// Add deviceReady flag to driver API to indicate
// when a device struct is initialised and populated
static LoRa_t *transceiver;
void LoRa_setTransceiver(LoRa_t *transceiver_) {
  transceiver = transceiver_;
}
LoRa_t *LoRa_getTransceiver() { return transceiver; }

// Optional switch for RF frontend
static GPIOpin_t *rfToggle;
void LoRa_setRfToggle(GPIOpin_t *rfToggle_) {
  rfToggle = rfToggle_;
}

/* ============================================================================================== */
/**
 * @brief LoRa transmit task.
 *
 * Handles transmission of data to the LoRa transceiver.
 *
 * This task blocks until a comment is received on the LoRa topic. Once received, the data is
 * transmitted and the task blocks until notified of transmit completion.
 **
 * ============================================================================================== */
void vLoRaTransmit(void *argument) {
  const TickType_t blockTime = portMAX_DELAY;
  CREATE_MESSAGE(txData, LORA_MSG_LENGTH);

  vLoRaTransmitHandle = xTaskGetCurrentTaskHandle();

  for (;;) {
    // Don't operate unless transceiver is ready
    if (transceiver == NULL)
      continue;

    // Wait to receive message to transmit
    BaseType_t result = WAIT_COMMENT(
      lora.public.commentInbox, // Read from LoRa topic comment queue
      &txData,                  // Store data in binary array
      portMAX_DELAY             // Block forever until comment is available
    );

    // Transmit data if successfully retrieved from queue
    if (result == pdTRUE) {

      if (rfToggle)
        // Toggle RF front-end for transmit
        rfToggle->reset(rfToggle);

      // Send data to transmit
      transceiver->transmit(transceiver, txData.data, txData.length);

      // Wait for notification from ISR
      xTaskNotifyWaitIndexed(1, 0, 0, NULL, blockTime);

      if (rfToggle)
        // Toggle RF front-end for receive
        rfToggle->set(rfToggle);

      // Continue receiving
      transceiver->startReceive(transceiver);
    }
  }
}

/* ============================================================================================== */
/**
 * @brief LoRa receive task.
 *
 * Handles receiving data from the LoRa transceiver.
 *
 * This task blocks until a notification is sent of a received packet. This can konly occur after
 * being set to receive mode, either by the transmit task or through a manual override in the
 * driver. Once notified, the packet data is read from the device and published to the LoRa topic.
 **
 * ============================================================================================== */
void vLoRaReceive(void *argument) {
  const TickType_t blockTime = portMAX_DELAY;

  CREATE_MESSAGE(rxData, LORA_MSG_LENGTH);

  vLoRaReceiveHandle = xTaskGetCurrentTaskHandle();

  for (;;) {
    // Don't operate unless transceiver is ready
    if (transceiver == NULL)
      continue;

    // Wait for notification from ISR
    xTaskNotifyWaitIndexed(1, 0, 0, NULL, blockTime);

    // Read received packet from transceiver
    rxData.length = transceiver->readReceive(transceiver, rxData.data, LORA_MSG_LENGTH);

    // Publish packet data to topic
    Topic_publish((PrivateTopic *)loraTopic, (uint8_t *)&rxData);
  }
}

/* ============================================================================================== */
/**
 * @brief LoRa Tx/Rx complete interrupt handler.
 *
 * Handles the external interrupt triggered by the Tx and Rx complete signals from the LoRa
 * transceiver. Upon interrupt, the appropriate task is notified according to the current LoRa
 * operating mode.
 **
 * ============================================================================================== */
void pubLoraInterrupt(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Exit if transceiver is not ready
  if (transceiver == NULL)
    goto LORA_NOT_READY;

  TaskHandle_t activeHandle = NULL;

  // Assign currently task handle for operation
  if (transceiver->currentMode == LORA_MODE_TX) {
    activeHandle = vLoRaTransmitHandle;
  } else if (transceiver->currentMode == LORA_MODE_RX) {
    activeHandle = vLoRaReceiveHandle;
  }

  // Exit if not initialised
  if (activeHandle == NULL)
    goto LORA_NOT_READY;

  // Notify active task for unblock
  xTaskNotifyIndexedFromISR(activeHandle, 1, 0, eNoAction, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

LORA_NOT_READY:
  // Clean-up and exit
  // TODO: Implement specific clear methods for
  //       Tx and Rx IRQs for better abstraction
  transceiver->clearIRQ(transceiver, 0xFF);
}

/** @} */
