#ifndef _LDATAACQUISITION_H
#define _LDATAACQUISITION_H

#include "FreeRTOS.h"
#include "event_groups.h"
#include "message_buffer.h"
#include "semphr.h"

#include "stdio.h"

#include "bmp581.h"
#include "dataframe.h"
#include "devicelist.h"
#include "kalmanfilter.h"
#include "kx134_1211.h"
#include "membuff.h"
#include "sensors.h"
#include "stateupdate.h"

#ifdef DUMMY
  #include "press.h"
#endif

void vLDataAcquisition(void *pvParameters);

#endif
