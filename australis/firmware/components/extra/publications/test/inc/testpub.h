// ALLOW FORMATTING
#ifndef TESTPUB_H
#define TESTPUB_H

#include <stdbool.h>

#include "topic.h"

#define TESTPUB_MESSAGE_MAX 32

typedef struct {
} PubTest_Context;

bool PubTest_startup(TopicHandle_t topic, void *context);

#endif
