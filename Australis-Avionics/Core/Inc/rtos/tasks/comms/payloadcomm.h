#ifndef _PAYLOADCOMM_H
#define _PAYLOADCOMM_H

#include "FreeRTOS.h"
#include "event_groups.h"
#include "message_buffer.h"
#include "stdio.h"

#include "can.h"
#include "dataframe.h"
#include "sx1272.h"
#include "params.h"
#include "stateupdate.h"

#define PAYLOAD_ACCEL_TOTAL 6

void vPayloadTransmit(void *);

#endif
