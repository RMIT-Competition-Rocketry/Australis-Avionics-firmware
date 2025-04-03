/* ===================================================================== *
 *                             LORA HANDLING                             *
 * ===================================================================== */

#include "stm32f439xx.h"

#include "FreeRTOS.h"
#include "event_groups.h"
#include "message_buffer.h"

#include "groups.h"
#include "loracomm.h"

#include "state.h"
#include "devicelist.h"

#include "gpiopin.h"

#include "accelerometer.h"
#include "gyroscope.h"
#include "lora.h"

extern EventGroupHandle_t xMsgReadyGroup;
extern MessageBufferHandle_t xLoRaTxBuff;
extern EventGroupHandle_t xSystemStatusGroup;

#define LORA_MSG_LENGTH 32

/**
 * @brief LoRa transmit task.
 *
 * Handles transmission of data to the SX1272 transceiver. It waits for the LoRa
 * module to be ready, then reads a message from the LoRa transmit buffer and
 * sends it via the SX1272. The ready flag is cleared after transmission.
 */
void vLoRaTransmit(void *argument) {
  const TickType_t blockTime = portMAX_DELAY;
  uint8_t rxData[LORA_MSG_LENGTH];

  LoRa_t *lora       = DeviceList_getDeviceHandle(DEVICE_LORA).device;
  GPIOpin_t rfToggle = GPIOpin_init(GPIOE, GPIO_PIN2, NULL);

  for (;;) {
    // Wait for SX1272 to be ready for transmission
    EventBits_t uxBits = xEventGroupWaitBits(xMsgReadyGroup, GROUP_MESSAGE_READY_LORA, pdFALSE, pdFALSE, blockTime);
    if ((uxBits & GROUP_MESSAGE_READY_LORA)) {
      // Wait to receive message in buffer
      size_t xReceivedBytes = xMessageBufferReceive(
          xLoRaTxBuff,
          (void *)rxData,
          sizeof(rxData),
          blockTime
      );
      // Transmit if message is available
      if (xReceivedBytes) {
        rfToggle.reset(&rfToggle); // Toggle RF front-end for transmit
        lora->transmit(lora, rxData);
        xEventGroupClearBits(xMsgReadyGroup, GROUP_MESSAGE_READY_LORA);
      }
    }
  }
}

/**
 * @brief LoRa sample task.
 *
 * Samples current sensor data from RAM every 250ms and queues it to be transmitted
 * by `vLoRaTransmit`. The task creates a LoRa packet containing accelerometer,
 * gyroscope, altitude, and velocity data, which is then appended to the transmission
 * queue.
 */
void vLoRaSample(void *argument) {
  const TickType_t xFrequency = pdMS_TO_TICKS(250);
  const TickType_t blockTime  = pdMS_TO_TICKS(125);

  Gyro_t *gyro                = DeviceList_getDeviceHandle(DEVICE_GYRO).device;
  Accel_t *lAccel             = DeviceList_getDeviceHandle(DEVICE_ACCEL_LOW).device;
  Accel_t *hAccel             = DeviceList_getDeviceHandle(DEVICE_ACCEL_HIGH).device;

  State *state                = State_getState();

  for (;;) {
    // Block until 250ms interval
    TickType_t xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    uint8_t systemStatus = xEventGroupGetBits(xSystemStatusGroup);

    uint8_t packet[LORA_MSG_LENGTH];

    int idx       = 0;
    packet[idx++] = state->flightState;
    memcpy(&packet[idx], lAccel->rawAccelData, lAccel->dataSize);
    memcpy(&packet[idx += lAccel->dataSize], hAccel->rawAccelData, hAccel->dataSize);
    memcpy(&packet[idx += hAccel->dataSize], gyro->rawGyroData, gyro->dataSize);
    memcpy(&packet[idx += gyro->dataSize], &state->altitude, sizeof(float));
    memcpy(&packet[idx += sizeof(float)], &state->velocity, sizeof(float));

    // Add packet to queue
    xMessageBufferSend(xLoRaTxBuff, &packet, LORA_MSG_LENGTH, blockTime);
  }
}

/**
 * @brief LoRa Tx complete interrupt handler.
 *
 * Handles the external interrupt triggered by the Tx complete signal from the
 * SX1272 transceiver. Upon interrupt, the LoRa ready flag is set in
 * `xMsgReadyGroup`.
 */
void EXTI1_IRQHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult;

  // Clear pending interrupt
  EXTI->PR |= (0x02);

  // Let the transmit task know it can continue
  xResult = xEventGroupSetBitsFromISR(
      xMsgReadyGroup,
      GROUP_MESSAGE_READY_LORA,
      &xHigherPriorityTaskWoken
  );

  if (xResult != pdFAIL)
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
