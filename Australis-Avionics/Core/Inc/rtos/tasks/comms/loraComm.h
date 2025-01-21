#ifndef _LORATRANSMIT_H
#define _LORATRANSMIT_H

#include "FreeRTOS.h"
#include "event_groups.h"
#include "groups.h"
#include "message_buffer.h"
#include "stm32f439xx.h"

#include "devices.h"
#include "BMP581.h"
#include "A3G4250D.h"
#include "KX134_1211.h"
#include "lora.h"
#include "stateUpdate.h"

void vLoRaTransmit(void *pvParameters);
void vLoRaSample(void *pvParameters);

typedef struct {
  ctxState state;
  KX134_1211 hAccel;
  KX134_1211 lAccel;
  A3G4250D gyro;
} ctxLoRaSample;

typedef struct {
  LoRa lora;
} ctxLoRaTransmit;

#endif
