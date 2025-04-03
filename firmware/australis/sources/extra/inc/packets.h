
/**
 * @author Matt Ricci
 *
 */

// ALLOW FORMATTING
#ifndef PACKETS_H
#define PACKETS_H

#include "stdint.h"

typedef struct {
  uint8_t id;    //!< Packet header ID
  uint8_t *data; //!< Packet payload
} LoRa_Packet;

#endif
