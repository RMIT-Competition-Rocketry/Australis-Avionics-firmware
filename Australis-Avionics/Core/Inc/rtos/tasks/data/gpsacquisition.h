#ifndef _GPSTRANSMIT_H
#define _GPSTRANSMIT_H

#include "FreeRTOS.h"
#include "event_groups.h"
#include "message_buffer.h"
#include "stdio.h"

#include "dataframe.h"
#include "devices.h"
#include "sam_m10q.h"
#include "sx1272.h"
#include "stateupdate.h"

#define GPS_RX_SIZE 128

void vGpsTransmit(void *);

#endif
