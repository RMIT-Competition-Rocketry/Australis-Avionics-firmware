
/**
 * @author Matt Ricci
 *
 */

// ALLOW FORMATTING
#ifndef PACKETS_H
#define PACKETS_H

#include "stdint.h"

#include "sx1272.h"

SX1272_Packet SX1272_AVData(
    uint8_t,
    uint8_t,
    uint8_t *,
    uint8_t *,
    uint8_t,
    uint8_t *,
    uint8_t,
    float,
    float
);
SX1272_Packet SX1272_GPSData(uint8_t, char *, char *, uint8_t);
SX1272_Packet SX1272_PayloadData(uint8_t, uint8_t, uint8_t *, uint8_t);

#endif
