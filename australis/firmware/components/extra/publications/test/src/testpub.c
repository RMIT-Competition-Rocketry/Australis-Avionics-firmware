/**************************************************************************************************
 * @file  testpub.c                                                                               *
 *                                                                                                *
 * @{                                                                                             *
 **************************************************************************************************/

#include <stddef.h>
#include <stdint.h>

#include "testpub.h"

#include "stm32f439xx.h"

#include "AustralisConfig.h"
#include "AustralisCore.h"
#include "_topic.h"
#include "topic.h"

#include "rtos/_messagedispatch.h"

#include "interrupts/interrupts.h"

static bool PubTest_handleComment(TopicHandle_t topic, void *data, size_t size);

/* =============================================================================== */
/**
 * @brief
 *
 **
 * =============================================================================== */
bool PubTest_startup(TopicHandle_t topic, void *context) {

  ASSERT(topic != NULL);

  topic->handleComment = PubTest_handleComment;
  topic->context       = context;

  return true;
}

/* =============================================================================== */
/**
 * @brief
 *
 **
 * =============================================================================== */
void PubTest_sendArticle(InterruptContext *interruptContext) {
  PubTest_Context *context = interruptContext->context;
}

#ifndef __DOXYGEN__

/* =============================================================================== */
/**
 * @brief
 *
 **
 * =============================================================================== */
static bool PubTest_handleComment(TopicHandle_t topic, void *data, size_t size) {
  uint8_t (*commentData)[size] = (uint8_t (*)[size])data;
  uint32_t x                   = DWT->CYCCNT;
  uint32_t y                   = SysTick->VAL;
  return false;
}

#endif

/** @} */
