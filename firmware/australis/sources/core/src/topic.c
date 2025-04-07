/***********************************************************************************
 * @file        topic.c                                                            *
 * @brief       Implementation of the Topic-based Publish/Subscribe system.        *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "_topic.h"
#include "topic.h"

#include "FreeRTOS.h"
#include "queue.h"

/* =============================================================================== */
/**
 * @brief Send a "comment" back to the originator of a topic.
 *
 * This function allows a task (typically a subscriber, but not necessarily)
 * to send data back to the task that created the topic.
 *
 * @param  topic   Pointer to the public `Topic` structure representing the target
 *                 topic. This structure must have been properly initialized.
 * @param  comment Pointer to the data buffer containing the comment message to send.
 *                 The size and structure of this data must be known by both the
 *                 sender and the topic originator's receiving task.
 *
 * @return `true`  if the comment was successfully sent to the topic's queue
 *                 (or if arguments were valid).
 * @return `false` if `topic` or `comment` is NULL.
 **
 * =============================================================================== */
bool Topic_comment(Topic *topic, uint8_t *comment) {
  // Early return if invalid argument
  if (topic == NULL || comment == NULL)
    return false;

  // Send comment data to author queue
  xQueueSend(topic->commentQueue, comment, 0);
  return true; // Returns true if args were valid and send was attempted
}

/* =============================================================================== */
/**
 * @brief   Publish an "article" to all discovered subscribers of a topic.
 *
 * Iterate through the array of subscriber queues that were discovered during
 * initialization. For each valid (non-NULL) queue handle found in the array,
 * attempt to send the provided `article` data to the queue without blocking.
 *
 * @param  topic   Pointer to the `PrivateTopic` structure representing the topic
 *                 to publish from. This contains the discovered subscriber list.
 * @param  article Pointer to the data buffer containing the article/message to publish.
 *                 The size and structure must be consistent with what subscribers
 *                 expect and the size used when subscribers created their queues.
 *
 * @return `true`  if the arguments (`topic`, `article`) are valid and the publishing
 *                 loop completed. Note: Does not guarantee successful delivery to
 *                 *all* subscribers (queues might be full, handles might be invalid
 *                 if subscriber didn't create queue correctly, etc.).
 * @return `false` if `topic` or `article` is NULL.
 **
 * =============================================================================== */
bool Topic_publish(PrivateTopic *topic, uint8_t *article) {
  // Early return if invalid argument
  if (topic == NULL || article == NULL)
    return false;

  // Calculate first and last addresses in subscription list
  const QueueHandle_t *start = topic->subscriptions;
  const QueueHandle_t *end   = topic->subscriptions + topic->numSubscriptions;

  // Send article data to each subscriber queue in the list
  for (const QueueHandle_t *entry = start; entry < end; entry++) {
    // Check if the retrieved handle is valid before sending
    // (Subscribers are responsible for creating their queues)
    if (entry != NULL)
      // Send data to the subscriber's queue (non-blocking)
      xQueueSend(*entry, article, 0);

    // Silently ignore if the handle is NULL
    // (subscriber might not have created queue yet/properly)
  }
  return true; // Returns true if args were valid and loop completed
}

/** @} */
