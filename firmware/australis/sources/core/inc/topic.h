/**
 * @file topic.h
 */

// ALLOW FORMATTING
#ifndef TOPIC_H
#define TOPIC_H

#include "FreeRTOS.h"
#include "queue.h"

#include "stdbool.h"

#define WAIT_ARTICLE xQueueReceive

typedef QueueHandle_t SubInbox_t;
typedef QueueHandle_t PubInbox_t;

/**
 * @brief Type definition for generic message struct.
 *
 * Typically variables of this type should not be defined
 * on their own, however the Message structure is useful
 * in parsing received messages from their raw binary.
 *
 * For initialisation of a message to be sent on a topic
 * refer to CREATE_MESSAGE.
 */
typedef struct {
  uint8_t length;
  uint8_t data[];
} Message;

/**
 * @brief Macro to declare a subscription to a specific topic.
 *
 * This macro declares a FreeRTOS queue handle (`subscription`) and places
 * it into a special linker section named ".<topic>_subscription". The
 * `CREATE_TOPIC` macro uses these sections to automatically discover
 * all subscribers for a given topic at runtime during initialization.
 *
 * @note  The subscriber task MUST create the queue assigned to the `subscription`
 *        variable. This macro only declares the handle and places it in the
 *        linker section for discovery.
 */
#define SUBSCRIBE_TOPIC(topic, inbox, length, size)                      \
  SubInbox_t __attribute__((section("." #topic "_subscription"))) inbox; \
  inbox = xQueueCreate(length, size);

/**
 * @brief Macro to define, on the stack, a named message struct
 *
 * This structure provides receivers of messages on a topic with
 * the associated data and its length for parsing.
 *
 * The message length is encoded in the first byte, with data as
 * a fixed length array member.
 */
#define CREATE_MESSAGE(name, length_) \
  struct {                            \
    uint8_t length;                   \
    uint8_t data[length_];            \
  } name = {.length = length_};

/**
 * @brief Public representation of a Topic.
 *
 * This structure provides the necessary handle for subscribers
 * (or others) to send data back to the topic authors.
 */
typedef struct PublicTopic {
  PubInbox_t commentInbox; //!< Queue handle used to send messages back to topic authors.
} Topic;

bool Topic_comment(Topic *topic, uint8_t *comment);

#endif
