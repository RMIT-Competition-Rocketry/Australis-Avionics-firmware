/**
 * @file groundcoms.h
 */

// ALLOW FORMATTING
#ifndef GROUNDCOMMS_H
#define GROUNDCOMMS_H

#define LORA_MESSAGE_INDEX_ID         0
#define LORA_MESSAGE_INDEX_BCAST_FLAG 2
#define LORA_MESSAGE_ID_GCS_REQUEST   0x01

void vGroundCommStateMachine(void *argument);

#endif
