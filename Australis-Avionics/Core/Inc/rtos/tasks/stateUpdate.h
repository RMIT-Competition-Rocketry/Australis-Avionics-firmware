#ifndef _STATE_H
#define _STATE_H

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "groups.h"
#include "handles.h"
#include "message_buffer.h"
#include "semphr.h"

#include "KX134_1211.h"
#include "can.h"
#include "dataframe.h"
#include "params.h"
#include "quaternion.h"
#include "sensors.h"
#include "slidingwindow.h"
#include "devices.h"

void vStateUpdate(void *pvParameters);

enum State {
  PRELAUNCH,
  LAUNCH,
  COAST,
  APOGEE,
  DESCENT
};

typedef struct {
  enum State currentState;
  Quaternion qRot;    // Global attitude quaternion
  float vAttitude[3]; // Attitude vector
  float zUnit[3];     // Z unit vector
  float cosine;       // Tilt angle cosine
  float tilt;         // Tilt angle

  // Flight dynamics state variables
  float altitude; // Current altitude
  float velocity; // Current vertical velocity
  SlidingWindow avgPress;
  SlidingWindow avgVel;
} ctxState;

typedef struct {
  ctxState *state;
  Handles *handles;
} ctxStateUpdate;

#endif
