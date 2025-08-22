/**
 * @author Matt Ricci
 * @addtogroup API API Reference
 * @{
 * @addtogroup
 * @}
 */

#ifndef INTERRUPT_SPI_H
#define INTERRUPT_SPI_H

#include "interrupts/interrupts.h"

void SPI_RegisterIRQ(InterruptIdentifier identifier, InterruptContext context);

/** @} */
#endif
