// ALLOW FORMATTING
#ifndef _MESSAGEDISPATCH_H
#define _MESSAGEDISPATCH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "topic.h"

typedef enum {
  MESSAGE_COMMENT,
  MESSAGE_ARTICLE
} MessageType;

typedef struct {
  TopicHandle_t topic;
  MessageType type;
  uint8_t data[];
} Message;

void vMessageDispatcher(void *argument);

bool queueArticle(TopicHandle_t topic, void *data, size_t size);

#endif
