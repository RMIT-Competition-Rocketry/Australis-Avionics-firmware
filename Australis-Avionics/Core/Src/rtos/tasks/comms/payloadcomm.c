/* ===================================================================== *
 *                                PAYLOAD                                *
 * ===================================================================== */

#include "payloadcomm.h"

extern MessageBufferHandle_t xLoRaTxBuff;
extern MessageBufferHandle_t xUsbTxBuff;
extern SemaphoreHandle_t xUsbMutex;
extern EventGroupHandle_t xSystemStatusGroup;

bool payloadCANRequest(unsigned int can, uint16_t id, unsigned int *out) {

  TIM6->ARR &= (~(TIM_ARR_ARR_Msk));
  TIM6->PSC &= (~(TIM_PSC_PSC_Msk));
  TIM6->SR &= ~(TIM_SR_UIF);
  TIM6->ARR = 49999;
  TIM6->PSC = 167;
  TIM6->CR1 |= TIM_CR1_CEN;

  // Request data from payload
  CAN_TX(can, 8, 0x00, 0x00, id);

  // Set output data
  struct CAN_RX_data payloadRx = {.CAN_number = can};

  // Wait until response is received with timeout
  while ((CAN_RX(&payloadRx) != 0x01) && (payloadRx.address != id)) {
    if (TIM6->SR & TIM_SR_UIF)
      return false;
  }

  // Set output data and return
  out[0] = payloadRx.dataL;
  out[1] = payloadRx.dataH;
  return true;
}

void vPayloadTransmit(void *argument) {
  const TickType_t xFrequency = pdMS_TO_TICKS(500);
  const TickType_t blockTime  = pdMS_TO_TICKS(250);

  for (;;) {
    // Block until 250ms interval
    TickType_t xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    uint8_t errCount = 0;

    unsigned int payloadState[2];
    errCount += !payloadCANRequest(CAN_PAYLOAD_AV, CAN_HEADER_PAYLOAD_STATUS, payloadState);

    unsigned int payloadAccel[2];
    errCount += !payloadCANRequest(CAN_PAYLOAD_AV, CAN_HEADER_PAYLOAD_ACCEL, payloadAccel);

    unsigned int payloadBaro1[2];
    errCount += !payloadCANRequest(CAN_PAYLOAD_AV, CAN_HEADER_PAYLOAD_BARO1, payloadBaro1);

    unsigned int payloadBaro2[2];
    errCount += !payloadCANRequest(CAN_PAYLOAD_AV, CAN_HEADER_PAYLOAD_BARO2, payloadBaro2);

    if (errCount > 2)
      xEventGroupSetBits(xSystemStatusGroup, GROUP_SYSTEM_STATUS_PAYLOAD);
    else
      xEventGroupClearBits(xSystemStatusGroup, GROUP_SYSTEM_STATUS_PAYLOAD);

    LoRa_Packet payloadData = LoRa_PayloadData(
        LORA_HEADER_PAYLOAD_DATA,
        (uint8_t)payloadState[0],
        (uint8_t *)payloadAccel,
        PAYLOAD_ACCEL_TOTAL
    );

    // Add packet to queue
    xMessageBufferSend(xLoRaTxBuff, &payloadData, LORA_MSG_LENGTH, blockTime);
  }
}
