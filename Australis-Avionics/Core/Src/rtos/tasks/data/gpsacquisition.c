/* ===================================================================== *
 *                                   GPS                                 *
 * ===================================================================== */

#include "gpsacquisition.h"

extern MessageBufferHandle_t xLoRaTxBuff;
extern MessageBufferHandle_t xUsbTxBuff;
extern StreamBufferHandle_t xGpsRxBuff;
extern SemaphoreHandle_t xUsbMutex;

uint8_t gpsRxBuff[GPS_RX_SIZE];
uint8_t gpsRxBuffIdx = 0;

void vGpsTransmit(void *argument) {
  const TickType_t xFrequency = pdMS_TO_TICKS(500);
  const TickType_t blockTime  = pdMS_TO_TICKS(250);
  char gpsString[100];

  SAM_M10Q_t *gps         = DeviceList_getDeviceHandle(DEVICE_GPS).device;
  UART_t *usb             = DeviceList_getDeviceHandle(DEVICE_UART_USB).device;
  enum State *flightState = StateHandle_getHandle("FlightState").state;

  for (;;) {
    // Block until 500ms interval
    TickType_t xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    // Send GPS poll message
    gps->base.print(&gps->base, GPS_PUBX_POLL);

    // Read string from UART Rx buffer, skip loop if empty
    if (!xStreamBufferReceive(xGpsRxBuff, (void *)&gpsString, gpsRxBuffIdx, blockTime))
      continue;

    struct GPS_Data gpsData;
    gps->decode(gps, gpsString, &gpsData);
    // usb->print(usb, gpsString);
    gpsRxBuffIdx = 0;

    #ifdef DEBUG
      //! @todo extract debug print to function
      //! @todo move debug function to new source file with context as parameter
      if ((xSemaphoreTake(xUsbMutex, pdMS_TO_TICKS(0))) == pdTRUE) {
        char debugStr[100];
        snprintf(debugStr, 100, "[GPS] %d:%d:%d\n\r", gpsData.hour, gpsData.minute, gpsData.second);
        xMessageBufferSend(xUsbTxBuff, (void *)debugStr, 100, 0);
        xSemaphoreGive(xUsbMutex);
      }
    #endif

    SX1272_Packet gpsPacket = SX1272_GPSData(
        LORA_HEADER_GPS_DATA,
        gpsData.latitude,
        gpsData.longitude,
        (*flightState << 4) | gpsData.lock
    );
    // Add packet to queue
    xMessageBufferSend(xLoRaTxBuff, &gpsPacket, LORA_MSG_LENGTH, blockTime);
  }
}

/* =============================================================================== */
/**
 * @brief Interrupt handler for USB UART receive.
 *
 * This handler is triggered when data is received via USB UART. It appends the
 * received byte to a circular buffer and sends it to a stream buffer for
 * processing by the USB receive task.
 **
 * =============================================================================== */
void USART3_IRQHandler() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Read in data from USART3
  while ((USART3->SR & USART_SR_RXNE) == 0);
  uint8_t rxData            = USART3->DR & 0xFF;

  gpsRxBuff[gpsRxBuffIdx++] = rxData;
  gpsRxBuffIdx %= GPS_RX_SIZE;

  // Send message to buffer on carriage return
  if (rxData == LINE_FEED) {
    xStreamBufferSendFromISR(xGpsRxBuff, (void *)gpsRxBuff, gpsRxBuffIdx, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}
