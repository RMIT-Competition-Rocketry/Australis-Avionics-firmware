#ifndef _USBTRANSCEIVE_H
#define _USBTRANSCEIVE_H

#include "FreeRTOS.h"
#include "message_buffer.h"
#include "stdint.h"
#include "stm32f439xx.h"

#include "devicelist.h"
#include "shell.h"
#include "uart.h"

#define USB_TX_SIZE 4096
#define USB_RX_SIZE 128

void vUsbReceive(void *);
void vUsbTransmit(void *);
void vShellExec(void *);

typedef struct {
  UART usb;
} ctxUsbTransmit;

typedef struct {
  UART usb;
  Shell shell;
} ctxUsbReceive;

#endif
