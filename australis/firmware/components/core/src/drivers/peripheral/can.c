/***********************************************************************************
 * @file        can.c                                                              *
 * @author      Matt Ricci                                                         *
 * @brief       Brief description of the file's purpose.                           *
 *                                                                                 *
 * @todo Cleanup CAN interface header and implementation.                          *
 ***********************************************************************************/

#include "can.h"
#include "gpiopin.h"

#include "stm32f439xx.h"

#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

static void _CAN_init(CAN_TypeDef *, CAN_Config *);

/* =============================================================================== */
/**
 * @brief  Initialiser for a CAN peripheral bus.
 *
 * @param  interface Pointer to the CAN_TypeDef struct representing the CAN interface.
 * @param  config    Pointer to CAN_Config struct for initial configuration.
 *                   This may be passed as \c NULL to initialise a default
 *                   configuration.
 *
 * @return can       Initialised CAN_t struct.
 **
 * =============================================================================== */
CAN_t CAN_init(CAN_TypeDef *interface, CAN_Config *config) {
  // Early return error struct if peripheral is NULL
  if (interface == NULL)
    return (CAN_t){.interface = NULL};

  // Initialise CAN struct with interface
  CAN_t can = {.interface = interface};

  // Update config and enable peripheral
  CAN_updateConfig(&can, config);

  // Initialise remaining parameters and methods
  can.transmit     = CAN_transmit;
  can.receive      = CAN_receive;
  can.updateConfig = CAN_updateConfig;

  return can;
}

// ALLOW FORMATTING
#ifndef __DOXYGEN__

/* =============================================================================== */
/**
 * @brief   Private initialiser for CAN registers.
 *
 * @param   interface Pointer to the CAN_TypeDef struct representing the CAN interface.
 * @param   config    Pointer to CAN_Config struct for initial configuration.
 *                    This may be passed as \c NULL to initialise a default
 *                    configuration. @see CAN_Config
 *
 * @return  @c NULL.
 **
 * =============================================================================== */
static void _CAN_init(CAN_TypeDef *interface, CAN_Config *config) {
  (void)interface;
  (void)config;
}

#endif

/* =============================================================================== */
/**
 * @brief
 *
 * @param
 *
 * @return
 **
 * =============================================================================== */
bool CAN_receive(CAN_t *can, CAN_Data *rxData) {

  volatile uint32_t *fifo = (can->interface == CAN1) ? &CAN1->RF0R : &CAN2->RF1R;
  uint8_t nFifo           = (can->interface == CAN1) ? 0 : 1;

  if (*fifo & CAN_RF0R_FMP0) {
    // Read frame identifier and received data
    rxData->id      = (can->interface->sFIFOMailBox[nFifo].RIR & 0xFFE00040) >> 21;
    rxData->data[0] = can->interface->sFIFOMailBox[nFifo].RDLR;
    rxData->data[0] = can->interface->sFIFOMailBox[nFifo].RDHR;

    // Update FIFO register
    *fifo |= 1 << 5;          // Release FIFO
    *fifo &= ~CAN_RF0R_FOVR0; // Clear overrun flag
    *fifo &= ~CAN_RF0R_FULL0; // Clear FIFO full flag
    return true;
  }

  return false; // No frame to receive
}

/* =============================================================================== */
/**
 * @brief
 *
 * @param
 *
 * @return
 **
 * =============================================================================== */
uint8_t CAN_transmit(CAN_t *can, CAN_Data *txData) {

  // Exit if no mailboxes are free
  bool mailboxFree = can->interface->TSR & (0b111 << CAN_TSR_TME0_Pos);
  if (!mailboxFree)
    return 0;

  uint8_t mailbox = (can->interface->TSR & CAN_TSR_CODE_Msk) >> CAN_TSR_CODE_Pos;

  // Set frame data
  can->interface->sTxMailBox[mailbox].TDHR = txData->data[1];
  can->interface->sTxMailBox[mailbox].TDLR = txData->data[0];
  can->interface->sTxMailBox[mailbox].TDTR = txData->length;

  /* Set frame identifier */
  can->interface->sTxMailBox[mailbox].TIR =
    (txData->id <= CAN_STID_MAX)           // Set id in:
      ? (txData->id << CAN_TI0R_STID_Pos)  // STID if within range
      : (txData->id << CAN_TI0R_EXID_Pos); // Otherwise in EXID

  // Request transmission
  can->interface->sTxMailBox[mailbox].TIR |= CAN_TI0R_TXRQ;

  // Add timer for timeout detection
  while (1) {
    if (can->interface->TSR & CAN_TSR_TXOK0)
      return 0; // Success
    else if (can->interface->TSR & CAN_TSR_TERR0) {
      can->interface->TSR |= CAN_TSR_ABRQ0;
      return 1; // TX error
    }
  }
  return 255;
}

/* =============================================================================== */
/**
 * @brief   Update CAN peripheral configuration
 * @details Uses the provided configuration to update the CAN registers and resets the
 *          associated peripheral.
 *          As with initialisation, passing \c NULL will set the default config.
 *
 * @param   can Pointer to CAN_t struct.
 *
 * @return  @c NULL.
 **
 * =============================================================================== */
void CAN_updateConfig(CAN_t *can, CAN_Config *config) {
  // Initialise config with default values if passed NULL.
  if (config == NULL) {
    // config = &CAN_CONFIG_DEFAULT;
  }

  // Update peripheral with new config
  can->config = *config;

  // Initialise CAN registers and enable peripheral
  _CAN_init(can->interface, config);
}

/* =============================================================================== */

void CANGPIO_config(void) {
  GPIO_Config cfg = GPIO_CONFIG_DEFAULT;
  cfg.afr         = GPIO_AF9;
  cfg.mode        = GPIO_MODE_AF;
  GPIOpin_init(GPIOA, GPIO_PIN11, &cfg);
  GPIOpin_init(GPIOA, GPIO_PIN12, &cfg);
}

void CAN_Peripheral_config(void) {
  CAN1->MCR |= CAN_MCR_RESET;                                    // Reset CAN
  while (CAN1->MCR & CAN_MCR_RESET);                             // Wait until reset

  CAN1->MCR &= ~CAN_MCR_SLEEP;                                   // Exit sleep
  while (CAN1->MSR & CAN_MSR_SLAK);

  CAN1->MCR |= CAN_MCR_INRQ;                                     // Request initialisation mode
  while (!(CAN1->MSR & CAN_MSR_INAK));

  CAN1->BTR                    &= (uint32_t)~(0xC37F03FF);       // clears all bit timing bits and disables loop back and silent mode
  CAN1->BTR                    |= 0x003f000f;                    // enters the Bitrate as 125kb/s
  CAN1->MCR                    |= (CAN_MCR_ABOM | CAN_MCR_AWUM); // Enable automatic bus-off management and wakeup

  CAN1->FMR                    |= 0x1;                           // sets the filter initialisation to 'on'
  CAN1->FM1R                   &= (uint32_t)~(0x1);              // sets to mask mode filter
  CAN1->FS1R                   |= 0x1;                           // sets to 32 bit mask, as the FR1/2 register is then for a single mask
  CAN1->FA1R                   &= 0xF0000000;                    // disable all filters

  CAN1->sFilterRegister[0].FR1  = 0;                             // assign filters so it will filter nothing
  CAN1->sFilterRegister[0].FR2  = 0;

  CAN1->FA1R                   |= 0x1;                           // enable the filter
  CAN1->FMR                    &= (uint32_t)~(0x1);              // take out of initialisation mode

  CAN1->MCR                    &= ~CAN_MCR_INRQ;                 // Enter normal mode
  while (CAN1->MSR & CAN_MSR_INAK);                              // Wait for normal mode

  CAN1->IER |= CAN_IER_FMPIE0;
}
