/***********************************************************************************
 * @file                                                                           *
 * @brief                                                                          *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "interrupts/interrupts.h"
#include "interrupts/interrupt_spi.h"

void Interrupt_registerCallback(InterruptIdentifier interrupt, InterruptContext context) {

  switch (interrupt) {
  case INTERRUPT_SPI1:
  case INTERRUPT_SPI2:
  case INTERRUPT_SPI3:
    SPI_RegisterIRQ(interrupt, context);
    break;
  default:
    break;
  }
}

/** @} */
