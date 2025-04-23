/***********************************************************************************
 * @file        packet.c                                                           *
 * @brief                                                                          *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "packet.h"

#include "stdbool.h"
#include "string.h"

static size_t _Packet_getSize(Packet *packet);

/* =============================================================================== */
/**
 * @brief
 *
 * @param
 *
 * @return
 **
 * =============================================================================== */
bool Packet_asBytes(Packet *packet, uint8_t *out, uint8_t size) {
  // Early exit with false if output array size is too small
  if (size < _Packet_getSize(packet))
    return false;

  // First byte is always packet identifier
  out[0] = packet->id;

  // Iterate through each field, copy data to output array
  for (int i = 1, j = 0; i < packet->length; i++) {
    memcpy(&out[i], packet->fields[j].data, packet->fields[j].size);
    j += packet->fields[j].size;
  }

  return true;
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
static size_t _Packet_getSize(Packet *packet) {
  size_t size = 1;

  for (int i = 0; i < packet->length; i++)
    size += packet->fields[i].size;

  return size;
}

/** @} */
