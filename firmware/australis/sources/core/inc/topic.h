/**
 * @file topic.h
 */

// ALLOW FORMATTING
#ifndef TOPIC_H
#define TOPIC_H

#include "FreeRTOS.h"
#include "queue.h"

#include "stdbool.h"

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
#define SUBSCRIBE_TOPIC(topic, subscription)                                      \
  QueueHandle_t __attribute__((section("." #topic "_subscription"))) subscription

/**
 * @brief Public representation of a Topic.
 *
 * This structure provides the necessary handle for subscribers
 * (or others) to send data back to the topic authors.
 */
typedef struct PublicTopic {
  QueueHandle_t commentQueue; //!< Queue handle used to send messages back to topic authors.
} Topic;

bool Topic_comment(Topic *topic, uint8_t *comment);

#endif
