/**
 * @file _topic.h
 */

// ALLOW FORMATTING
#ifndef PRIVATETOPIC_H
#define PRIVATETOPIC_H

#include "topic.h"

#include "stdbool.h"

#define WAIT_COMMENT xQueueReceive

/**
 * @brief Macro to declare a topic instance.
 *
 * This macro performs several key actions for creating a publishable topic:
 *   1. Declares a static `PrivateTopic` variable named `topic`.
 *   2. Declares external symbols defined by the linker script to mark the
 *      beginning and end of the memory section containing all subscribtion
 *      queues. (`__<topic>_subscriptions_start`, `__<topic>_subscriptions_end`)
 */
#define DECLARE_TOPIC(topic)                                 \
  PrivateTopic topic;                                        \
  extern const SubInbox_t __##topic##_subscriptions_start[]; \
  extern const SubInbox_t __##topic##_subscriptions_end[];

/**
 * @brief Macro to initialise a declared topic instance.
 *
 * This macro:
 *   1. Defines a static function `startTopic` decorated with the constructor
 *      attribute. This ensures `startTopic` runs automatically before `main()`.
 *   2. Inside `startTopic`:
 *      - Creates the FreeRTOS queue (`commentInbox`) for receiving "comments"
 *        sent via `Topic_comment`.
 *      - Determines the start address and number of subscriber queues by
 *        inspecting the linker-defined start/end symbols.
 *      - Stores this information in the `topic` variable.
 */
#define INIT_TOPIC(topic, commentInboxSize, messageSize)                                    \
  topic.public.commentInbox = xQueueCreate(commentInboxSize, messageSize);                  \
  if (topic.public.commentInbox == NULL)                                                    \
    while (1);                                                                              \
  topic.subscriptions    = __##topic##_subscriptions_start;                                 \
  topic.numSubscriptions = __##topic##_subscriptions_end - __##topic##_subscriptions_start;

/**
 * @brief Macro to define and initialize a topic instance.
 *
 * This macro combines the DECLARE_TOPIC and INIT_TOPIC macros to create
 * a topic in one step and initialise it automatically at run-time prior
 * to main function entry.
 *
 * NOTE: If used, this macro must be placed at global scope within a file
 *
 * TODO: Replace while(1) with assert when printf redirect is implemented
 */
#define CREATE_TOPIC(topic, commentInboxSize, messageSize) \
  DECLARE_TOPIC(topic)                                     \
  __attribute__((constructor)) static void startTopic() {  \
    INIT_TOPIC(topic, commentInboxSize, messageSize)       \
  }

/**
 * @brief Internal representation of a Topic instance.
 *
 * Holds the complete state for a topic managed by the publisher.
 * This structure is primarily manipulated by the `CREATE_TOPIC` macro
 * and the `Topic_publish` function.
 */
typedef struct PrivateTopic {
  Topic public;                    //!< Public topic interface.
  const SubInbox_t *subscriptions; //!< Subscription inbox array pointer.
  size_t numSubscriptions;         //!< Number of subscriptions to the topic.
} PrivateTopic;

bool Topic_publish(PrivateTopic *topic, uint8_t *article);

#endif
