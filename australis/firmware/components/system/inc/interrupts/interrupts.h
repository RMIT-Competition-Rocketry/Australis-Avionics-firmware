/**
 * @author Matt Ricci
 * @addtogroup API API Reference
 * @{
 * @addtogroup
 * @}
 */

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "stm32f439xx.h"

/**
 * @brief
 * @details
 */
typedef void (*InterruptCallback)(void *context);

/**
 * @brief
 * @details
 */
typedef struct {
  InterruptCallback callback;
  void *context;
} InterruptContext;

/**
 * @brief
 * @details
 */
typedef enum {
  INTERRUPT_SPI1,
  INTERRUPT_SPI2,
  INTERRUPT_SPI3,
  INTERRUPT_INVALID
} InterruptIdentifier;

/** @} */
#endif
