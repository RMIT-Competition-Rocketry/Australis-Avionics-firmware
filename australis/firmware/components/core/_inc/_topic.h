/**
 * @file _topic.h
 */

// ALLOW FORMATTING
#ifndef PRIVATETOPIC_H
#define PRIVATETOPIC_H

#include <stddef.h>
#include <stdint.h>

#include "topic.h"
#include "stdbool.h"

#define WAIT_COMMENT xMessageBufferReceive

typedef bool (*MessageHandler)(TopicHandle_t topic, void *data, size_t size);
typedef bool (*AcquisitionHandler)(TopicHandle_t topic);

typedef struct Subscription {
  SubHandle_t next;
  SubInbox_t inbox;
} Subscription;

/**
 * @brief Internal representation of a Topic instance.
 *
 */
typedef struct Topic {
  const char *name;               //!<
  void *context;                  //!<
  bool initialised;               //!<
  TopicHandle_t parent;           //!<
  SubHandle_t head;               //!<
  SubHandle_t tail;               //!<
  size_t numSubscriptions;        //!< Number of subscriptions to the topic.
  MessageHandler handleComment;   //!<
  AcquisitionHandler acquireData; //!<
} Topic;

bool Topic_publish(TopicHandle_t topic, uint8_t *article, size_t articleSize);

#endif
