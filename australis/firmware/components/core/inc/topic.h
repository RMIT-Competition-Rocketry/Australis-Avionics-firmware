/**
 * @file topic.h
 */

// ALLOW FORMATTING
#ifndef TOPIC_H
#define TOPIC_H

#include <stdbool.h>
#include <stddef.h>

#include "FreeRTOS.h" // IWYU pragma: keep
#include "message_buffer.h"

#define WAIT_ARTICLE xMessageBufferReceive

typedef MessageBufferHandle_t SubInbox_t;

typedef struct Subscription *SubHandle_t;
typedef struct Topic *TopicHandle_t;

typedef bool (*StartupHandler)(TopicHandle_t topic, void *context);

TopicHandle_t Topic_new(const char *name, void *context, StartupHandler startup);

TopicHandle_t Topic_get(const char *name);
SubHandle_t Topic_subscribeByHandle(TopicHandle_t topic);
SubHandle_t Topic_subscribeByName(const char *name);

#endif
