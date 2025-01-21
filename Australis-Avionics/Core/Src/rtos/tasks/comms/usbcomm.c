/* ===================================================================== *
 *                             UART HANDLING                             *
 * ===================================================================== */

#include "usbcomm.h"

extern MessageBufferHandle_t xUsbTxBuff;
extern MessageBufferHandle_t xUsbRxBuff;

uint8_t usbRxBuff[USB_RX_SIZE];
uint8_t usbRxBuffIdx = 0;

/* =============================================================================== */
/**
 * @brief USB transmit task for handling UART output.
 **
 * =============================================================================== */
void vUsbTransmit(void *argument) {
  const TickType_t timeout = portMAX_DELAY;
  uint8_t rxData[100];

  UART *usb = DeviceHandle_getHandle("USB").device;

  for (;;) {
    // Read byte from UART Tx buffer, skip loop if empty
    if (!xMessageBufferReceive(xUsbTxBuff, (void *)rxData, 100, timeout))
      continue;

    usb->print(usb, (char *)rxData);
  }
}

/* =============================================================================== */
/**
 * @brief USB receive task for handling UART input.
 *
 * This task continuously reads data from the UART receive buffer.
 * Each byte is sent back to the host for display. On detecting a
 * carriage return (`<Enter>`), the task processes the command stored
 * in the buffer, sends a newline character for display, and resets
 * the buffer for the next command.
 *
 * This task additionally handles specific control characters:
 * 	 - `<Ctrl-C>` clears the terminal.
 * 	 - `<Backspace>` erases the last character.
 * @todo Move calls to shell scripts to new thread
 **
 * =============================================================================== */
void vUsbReceive(void *argument) {
  const TickType_t timeout = portMAX_DELAY;
  uint8_t rxData;

  UART *usb    = DeviceHandle_getHandle("USB").device;
  Shell *shell = argument;

  for (;;) {
    // Read byte from UART Rx buffer, skip loop if empty
    if (!xStreamBufferReceive(xUsbRxBuff, (void *)&rxData, 1, timeout))
      continue;

    // Send byte back for display
    usb->send(usb, rxData);

    // Process command and reset buffer on <Enter> input
    if (rxData == CARRIAGE_RETURN) {
      usb->print(usb, "\n");              // Send newline back for display
      usbRxBuff[usbRxBuffIdx - 1] = '\0'; // Replace carriage return with null terminator
      shell->runTask(shell, usbRxBuff);   // Run shell program as task
      usbRxBuffIdx = 0;                   // Reset buffer
    }

    // Clear terminal on <Ctrl-c> input
    else if (rxData == SIGINT) {
      shell->clear(shell);
      usbRxBuffIdx = 0;
    }

    // Erase character and move cursor backwards on <BS> input
    else if (rxData == BACKSPACE) {
      usb->print(usb, " \b");
      if (usbRxBuffIdx)
        usbRxBuffIdx -= 2;
    }
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
void USART6_IRQHandler() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Read in data from USART6
  while ((USART6->SR & USART_SR_RXNE) == 0);
  uint8_t rxData            = USART6->DR & 0xFF;

  usbRxBuff[usbRxBuffIdx++] = rxData;
  usbRxBuffIdx %= USB_RX_SIZE;

  xStreamBufferSendFromISR(xUsbRxBuff, (void *)&rxData, 1, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
