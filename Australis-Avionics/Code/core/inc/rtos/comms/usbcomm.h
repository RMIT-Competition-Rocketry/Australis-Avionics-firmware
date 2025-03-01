#ifndef _USBTRANSCEIVE_H
#define _USBTRANSCEIVE_H

#define USB_TX_SIZE 4096
#define USB_RX_SIZE 128

void vUsbReceive(void *);
void vUsbTransmit(void *);
void vShellExec(void *);

#endif
