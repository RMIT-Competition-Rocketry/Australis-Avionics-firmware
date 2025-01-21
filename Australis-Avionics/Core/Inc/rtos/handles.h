#ifndef _HANDLES_H
#define _HANDLES_H

#include "FreeRTOS.h"
#include "event_groups.h"

typedef struct {
  TaskHandle_t xHDataAcquisitionHandle;
  TaskHandle_t xLDataAcquisitionHandle;
  TaskHandle_t xFlashBufferHandle;
  TaskHandle_t xStateUpdateHandle;
  TaskHandle_t xLoRaTransmitHandle;
  TaskHandle_t xLoRaSampleHandle;
  TaskHandle_t xUsbReceiveHandle;
  TaskHandle_t xUsbTransmitHandle;
	TaskHandle_t xPayloadTransmitHandle;
  TaskHandle_t xGpsTransmitHandle;
  TaskHandle_t xIdleHandle;
} Handles;

#endif
