/***********************************************************************************
 * @file        can.c                                                              *
 * @author      Matt Ricci                                                         *
 * @addtogroup  CAN                                                                *
 * @brief       Brief description of the file's purpose.                           *
 *                                                                                 *
 * @todo Cleanup CAN interface header and implementation.                          *
 ***********************************************************************************/

#include "can.h"

/**
 * @addtogroup CAN
 * @brief CAN interface
 * @{
 */

uint8_t CAN_RX(struct CAN_RX_data *CAN) {
  if (CAN->CAN_number == 1) {
    if (CAN1->RF1R & 0x3) {                                           // checks to see if there was a CAN message recieved
      CAN->address  = (CAN1->sFIFOMailBox[1].RIR & 0xFFE00040) >> 21; // extracts the address from the CAN message and stores it into the CAN
      CAN->dataL    = CAN1->sFIFOMailBox[1].RDLR;                     // extracts the LSB 4 bytes
      CAN->dataH    = CAN1->sFIFOMailBox[1].RDHR;                     // extracts the MSB 4 bytes
      CAN1->RF0R   |= 1 << 5;                                         // release FIFO
      // clear bits 3 and 4 (indicating the mailboxes are full)
      CAN1->RF0R &= (unsigned int)~(1 << 3);
      CAN1->RF0R &= (unsigned int)~(1 << 4);
      return 1;
    } else
      return 0;                      // returns 0 if nothing recieved
  } else if (CAN->CAN_number == 2) { // samething but for CAN2
    if (CAN2->RF1R & 0x3) {
      CAN->address  = (CAN2->sFIFOMailBox[1].RIR & 0xFFE00040) >> 21;
      CAN->dataL    = CAN2->sFIFOMailBox[1].RDLR;
      CAN->dataH    = CAN2->sFIFOMailBox[1].RDHR;
      CAN2->RF1R   |= 1 << 5;
      // clear bits 3 and 4 (indicating the mailboxes are full)
      CAN2->RF1R &= (unsigned int)~(1 << 3);
      CAN2->RF1R &= (unsigned int)~(1 << 4);
      return 1;
    } else
      return 0; // returns 0 if nothing recieved
  } else
    return 255;
}

uint8_t CAN_TX(uint8_t CAN, uint8_t data_length, unsigned int dataH, unsigned int dataL, unsigned int address) {
  uint8_t mailbox = find_empty_CAN_TX_mailbox(CAN);
  if (mailbox == 255)
    return 250;
  if (CAN == 1) {
    CAN1->sTxMailBox[mailbox].TDHR = 0;                         // stores the dataH into the Mailbox to transmit
    CAN1->sTxMailBox[mailbox].TDLR = 0;                         // stores the dataL into the Mailbox to transmit
    CAN1->sTxMailBox[mailbox].TDTR = 0;                         // puts in the data length
    CAN1->sTxMailBox[mailbox].TDHR = (unsigned int)dataH;       // stores the dataH into the Mailbox to transmit
    CAN1->sTxMailBox[mailbox].TDLR = (unsigned int)dataL;       // stores the dataL into the Mailbox to transmit
    CAN1->sTxMailBox[mailbox].TDTR = (unsigned int)data_length; // puts in the data length
    // unsigned int CAN_TIR = CAN1->sTxMailBox[mailbox].TIR;
    // CAN_TIR &=	(unsigned int)~(0xFFE00000);
    // CAN_TIR |= (uint8_t) (address << 21);// enters in the CAN identifer
    CAN1->sTxMailBox[mailbox].TIR = 0;
    CAN1->sTxMailBox[mailbox].TIR = (address << 21); // enters in the CAN identifer
                                                     // CAN1->sTxMailBox[mailbox].TIR |= CAN_TIR;
    CAN1->sTxMailBox[mailbox].TIR |= (1 << 0);       // requested transmission
    while (1)                                        // add timer in here for timeout
    {
      if ((CAN1->TSR & (1 << (1))))
        return 0;                                    // successful
      else if ((CAN1->TSR & (1 << 3)))
        CAN1->TSR |= (unsigned int)((1 << 7));
      CAN1->TSR |= (unsigned int)((1 << 7));         // used to abort the transmissions if there is an error
      return 1;                                      // TX error
    }
    return 255;                                      // timeout error
  }
  if (CAN == 2) {
    CAN2->sTxMailBox[mailbox].TDHR  = dataH;
    CAN2->sTxMailBox[mailbox].TDLR  = dataL;
    CAN2->sTxMailBox[mailbox].TDTR  = data_length;
    CAN2->sTxMailBox[mailbox].TIR   = address << 21;
    CAN2->sTxMailBox[mailbox].TIR  |= (1 << 0); // requested transmission
    while (1)                                   // add timer in here for timeout
    {
      if ((CAN2->TSR & (1 << (1))))
        return 0;                               // successful
      else if ((CAN2->TSR & (1 << 3))) {
        CAN2->TSR |= (unsigned int)((1 << 7));
        CAN2->TSR |= (unsigned int)((1 << 7));
        return 1; // TX error
      }
    }
    return 255;   // timeout error
  } else
    return 100;
}

uint8_t find_empty_CAN_TX_mailbox(uint8_t CAN) {
  return 0; // it is simply going to reload to the single mailbox and send
  volatile uint32_t *CAN_address = &CAN1->TSR;
  if (!((CAN == 1) || (CAN == 2)))
    return 100;
  if (CAN == 2) {
    CAN_address = &CAN2->TSR;
  }

  for (uint8_t i = 0; i < 3; i++) {
    if (*CAN_address & (1 << (i + 26))) {
      return i; // returns the mailbox number
    }
  }
  return 255;   // if mailboxes are all full
}

void CANGPIO_config() {
  // config AF functions
  // CAN1 RX PA11 TXPA12 CAN2 RX PB12 TX PB13 both AF9
  // Configure to Alternate Functions
  GPIOA->MODER &= (uint32_t)~0x3C00000;
  GPIOB->MODER &= (uint32_t)~0xF000000;
  GPIOA->MODER |= 0x2800000;
  GPIOB->MODER |= 0xA000000;
  // Configure the TX to be push-pull
  GPIOA->OTYPER &= (uint32_t)~0x1000;
  GPIOB->OTYPER &= (uint32_t)~0x2000;
  // Output speed
  GPIOA->OSPEEDR |= 0x3000000;
  GPIOB->OSPEEDR |= 0xC000000;
  // set the pull-ups
  GPIOA->PUPDR &= 0xC00000;
  GPIOB->PUPDR &= 0x3000000;
  // GPIOB->PUPDR |= 0x1000000;
  // GPIOA->PUPDR |= 0x400000;
  //  Remap the AFIO
  GPIOA->AFR[1] &= (uint32_t)~(0xFF000);
  GPIOA->AFR[1] |= (0x99000);
  GPIOB->AFR[1] &= (uint32_t)~(0xFF0000);
  GPIOB->AFR[1] |= (0x990000);
}

void CAN_Peripheral_config() {
  // volatile uint32_t* CAN = CAN1->MCR;
  // for (uint8_t x = 0; x > 2; x++){
  CAN1->MCR |= 0x8000;                     // reset CAN
  while (((CAN1->MCR & (CAN_MCR_RESET)))); // wait until reset

  CAN1->MCR |= 0x1;
  while (!(CAN1->MSR & 1));                // change
  CAN1->BTR &= (uint32_t)~(0xC37F03FF);    // clears all bit timing bits and disables loop back and silent mode
  CAN1->BTR |= 0x22B0014;                  // enters the Bitrate as 125kb/s
  CAN1->MCR &= ~(CAN_MCR_SLEEP);           // Clear sleep bit
  CAN1->MCR &= (uint32_t)~(1 << 0);        // places CAN into normal mode
  while ((CAN1->MSR & (1 << 0)));          // change for MSR

  CAN1->FMR   |= 0x1;                      // sets the filter initialisation to 'on'
  CAN1->FM1R  &= (uint32_t)~(0x1);         // sets to mask mode filter
  CAN1->FS1R  |= 0x1;                      // sets to 32 bit mask, as the FR1/2 register is then for a single mask
  CAN1->FFA1R |= 0x1;                      // Assigned to FIFO 1
  CAN1->FS1R  |= (1 << 25);                // sets to 32 bit mask, as the FR1/2 register is then for a single mask
  CAN1->FFA1R |= (1 << 25);                // Assigned to FIFO 1
  CAN1->FA1R  &= 0xF0000000;               // disable all filters
  //
  CAN1->sFilterRegister[0].FR1   = 0;                // assign filters so it will filter nothing
  CAN1->sFilterRegister[0].FR2   = 0;
  CAN1->sFilterRegister[25].FR1  = 0;                // assign filters so it will filter nothing
  CAN1->sFilterRegister[25].FR2  = 0;

  CAN1->FA1R                    |= 0x1;              // enable the filter
  CAN1->FA1R                    |= 1 << 25;          // enable the filter
  CAN1->FMR                     &= (uint32_t)~(0x1); // take out of initialisation mode

  // this makes that all CAN messages will go to FIFO1
  CAN2->MCR |= 0x8000;                     // reset CAN
  while (((CAN2->MCR & (CAN_MCR_RESET)))); // wait until reset

  CAN2->MCR |= 0x1;
  while (!(CAN2->MSR & 1));                // change
  CAN2->BTR &= (uint32_t)~(0xC37F03FF);    // clears all bit timing bits and disables loop back and silent mode
  CAN2->BTR |= 0x22B0014;                  // enters the Bitrate
  CAN2->MCR &= ~(CAN_MCR_SLEEP);           // Clear sleep bit
  CAN2->MCR &= (uint32_t)~(1 << 0);        // places CAN into normal mode
  while ((CAN2->MSR & (1 << 0)));          // change for MSR
  CAN2->FMR &= (uint32_t)~(0x1);           // take out of initialisation mode
  CAN2->IER |= CAN_IER_FMPIE1;

  // CAN2->FMR |= 0x1; // sets the filter initialisation to 'on'
  // CAN2->FM1R &= (uint32_t)~(0x1); // sets to mask mode filter
  // CAN2->FS1R |= (1 << 25); // sets to 32 bit mask, as the FR1/2 register is then for a single mask
  // CAN2->FFA1R |= (1 << 25); // Assigned to FIFO 1
  // CAN2->FA1R &= 0xF0000000; // disable all filters
  // CAN2->sFilterRegister[0].FR1 = 0;// assign filters so it will filter nothing
  // CAN2->sFilterRegister[0].FR2 = 0;
  // CAN2->FA1R |= 0x1; // enable the filter

  // this makes that all CAN messages will go to FIFO1
}
