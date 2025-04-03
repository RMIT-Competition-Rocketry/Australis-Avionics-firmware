/**
 * @author Matt Ricci
 * @addtogroup API
 * @{
 * @addtogroup CAN
 * @}
 */

#ifndef _CAN_H
#define _CAN_H

#include "stdint.h"
#include "stm32f439xx.h"

/**
 * @ingroup CAN
 * @addtogroup CAN_Interface Interface
 * @{
 */

struct CAN_RX_data {
  unsigned int dataL;   // data high register
  unsigned int dataH;   // data low register
  unsigned int address; // CAN identifer
  uint8_t CAN_number;   // either CAN2 or CAN1
};

void CANGPIO_config();
void CAN_Peripheral_config();
uint8_t find_empty_CAN_TX_mailbox(uint8_t);
uint8_t CAN_TX(uint8_t, uint8_t, unsigned int, unsigned int, unsigned int);
uint8_t CAN_RX(struct CAN_RX_data *);

#endif
