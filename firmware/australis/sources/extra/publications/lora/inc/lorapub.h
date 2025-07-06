// ALLOW FORMATTING
#ifndef LORAPUB_H
#define LORAPUB_H

#include "topic.h"
#include "lora.h"
#include "gpiopin.h"

extern Topic *loraTopic;

void vLoRaTransmit(void *pvParameters);
void vLoRaReceive(void *pvParameters);
void LoRa_setRfToggle(GPIOpin_t *rfToggle);

LoRa_t *LoRa_getTransceiver();
void LoRa_setTransceiver(LoRa_t *transceiver);
void pubLoraInterrupt();

#endif
