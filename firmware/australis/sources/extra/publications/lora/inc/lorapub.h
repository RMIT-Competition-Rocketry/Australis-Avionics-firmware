// ALLOW FORMATTING
#ifndef LORAPUB_H
#define LORAPUB_H

#include "topic.h"

#define LORA_MSG_LENGTH 32

extern Topic *loraTopic;

void vLoRaTransmit(void *pvParameters);
void vLoRaReceive(void *pvParameters);

#endif
