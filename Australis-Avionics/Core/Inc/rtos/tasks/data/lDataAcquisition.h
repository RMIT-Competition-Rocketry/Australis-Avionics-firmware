#ifndef _LDATAACQUISITION_H
#define _LDATAACQUISITION_H

#include "FreeRTOS.h"
#include "event_groups.h"
#include "message_buffer.h"
#include "semphr.h"

#include "stdio.h"

#include "BMP581.h"
#include "KX134_1211.h"
#include "dataframe.h"
#include "kalmanfilter.h"
#include "membuff.h"
#include "sensors.h"
#include "stateUpdate.h"

#ifdef DUMMY
  #include "press.h"
#endif

void vLDataAcquisition(void *pvParameters);

typedef struct {
  ctxState *state;
  MemBuff *mem;
} ctxLDataAcquisition;

#endif
