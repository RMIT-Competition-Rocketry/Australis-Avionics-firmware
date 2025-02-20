#ifndef __HDATAACQUISITION_H
#define __HDATAACQUISITION_H

#include "FreeRTOS.h"
#include "event_groups.h"
#include "groups.h"
#include "message_buffer.h"
#include "semphr.h"

#include "stdio.h"

#include "a3g4250d.h"
#include "dataframe.h"
#include "devicelist.h"
#include "kx134_1211.h"
#include "membuff.h"
#include "sensors.h"
#include "stateupdate.h"

#ifdef DUMMY
  #include "accelX.h"
  #include "accelY.h"
  #include "accelZ.h"
  #include "gyroX.h"
  #include "gyroY.h"
  #include "gyroZ.h"
#endif

void vHDataAcquisition(void *pvParameters);

#endif
