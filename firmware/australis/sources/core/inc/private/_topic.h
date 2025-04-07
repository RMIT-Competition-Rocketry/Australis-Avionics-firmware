/**
 * @file _topic.h
 */

// ALLOW FORMATTING
#ifndef PRIVATETOPIC_H
#define PRIVATETOPIC_H

#include "topic.h"

#include "stdbool.h"

/**
 * @brief Macro to define and initialize a topic instance.
 *
 * This macro performs several key actions for creating a publishable topic:
 *   1. Declares a static `PrivateTopic` variable named `topic`.
 *   2. Declares external symbols defined by the linker script to mark the
 *      beginning and end of the memory section containing all subscribtion
 *      queues. (`__<topic>_subscriptions_start`, `__<topic>_subscriptions_end`)
 *   3. Defines a static function `startTopic` decorated with the constructor
 *      attribute. This ensures `startTopic` runs automatically before `main()`.
 *   4. Inside `startTopic`:
 *      - Creates the FreeRTOS queue (`commentQueue`) for receiving "comments"
 *        sent via `Topic_comment`.
 *      - Determines the start address and number of subscriber queues by
 *        inspecting the linker-defined start/end symbols.
 *      - Stores this information in the `topic` variable.
 *
 * TODO: Replace while(1) with assert when printf redirect is implemented
 */
#define CREATE_TOPIC(topic, commentQueueLength, messageSize)                                  \
  PrivateTopic topic;                                                                         \
  extern const QueueHandle_t __##topic##_subscriptions_start[];                               \
  extern const QueueHandle_t __##topic##_subscriptions_end[];                                 \
  __attribute__((constructor)) static void startTopic() {                                     \
    topic.public.commentQueue = xQueueCreate(commentQueueLength, messageSize);                \
    if (topic.public.commentQueue == NULL)                                                    \
      while (1);                                                                              \
    topic.subscriptions    = __##topic##_subscriptions_start;                                 \
    topic.numSubscriptions = __##topic##_subscriptions_end - __##topic##_subscriptions_start; \
  }

/**
 * @brief Internal representation of a Topic instance.
 *
 * Holds the complete state for a topic managed by the publisher.
 * This structure is primarily manipulated by the `CREATE_TOPIC` macro
 * and the `Topic_publish` function.
 */
typedef struct PrivateTopic {
  Topic public;                       //!< Public topic interface.
  const QueueHandle_t *subscriptions; //!< Subscription queue array pointer.
  size_t numSubscriptions;            //!< Number of subscriptions to the topic.
} PrivateTopic;

bool Topic_publish(PrivateTopic *topic, uint8_t *article);

#endif
