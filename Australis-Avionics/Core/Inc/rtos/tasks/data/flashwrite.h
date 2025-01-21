#ifndef _FLASHBUFFER_H
#define _FLASHBUFFER_H

#include "FreeRTOS.h"
#include "event_groups.h"
#include "stdbool.h"

#include "devices.h"
#include "flash.h"
#include "membuff.h"
#include "stateupdate.h"

typedef struct {
  enum State *currentState;
  MemBuff *mem;
} ctxIdle;

typedef struct {
  enum State *currentState;
  MemBuff *mem;
} ctxFlashBuffer;

void vIdle(void *pvParameters);
void vFlashBuffer(void *pvParameters);

#endif
