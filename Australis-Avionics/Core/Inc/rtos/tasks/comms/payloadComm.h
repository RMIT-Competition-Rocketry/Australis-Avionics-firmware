#ifndef _PAYLOADCOMM_H
#define _PAYLOADCOMM_H

#include "FreeRTOS.h"
#include "event_groups.h"
#include "message_buffer.h"
#include "stdio.h"

#include "params.h"
#include "stateUpdate.h"
#include "dataframe.h"
#include "can.h"
#include "lora.h"

#define PAYLOAD_ACCEL_TOTAL 6

void vPayloadTransmit(void *);

#endif
