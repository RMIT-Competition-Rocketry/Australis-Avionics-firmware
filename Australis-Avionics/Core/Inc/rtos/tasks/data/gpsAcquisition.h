#ifndef _GPSTRANSMIT_H
#define _GPSTRANSMIT_H

#include "FreeRTOS.h"
#include "event_groups.h"
#include "message_buffer.h"
#include "stdio.h"

#include "stateUpdate.h"
#include "dataframe.h"
#include "devices.h"
#include "gps.h"
#include "lora.h"

#define GPS_RX_SIZE     128

void vGpsTransmit(void *);

#endif
