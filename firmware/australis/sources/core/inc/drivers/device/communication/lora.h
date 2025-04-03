/**
 * @author Matt Ricci
 * @ingroup API
 * @addtogroup LoRa
 * @{
 */

// ALLOW FORMATTING
#ifndef LORA_H
#define LORA_H

#include "stdint.h"
#include "stdbool.h"

/**
 * @brief   LoRa base operating mode enum
 */
typedef enum {
  LORA_MODE_TX,   // Transmission mode
  LORA_MODE_RX,   // Continuous receive mode
  LORA_MODE_OTHER // Driver defined operating mode
} LoRa_Mode;

typedef struct LoRa {
  LoRa_Mode currentMode;                                  //!< Current operating mode.
  void (*transmit)(struct LoRa *, uint8_t *);             //!< LoRa transmit method.
  void (*startReceive)(struct LoRa *);                    //!< LoRa continuous receive method.
  bool (*readReceive)(struct LoRa *, uint8_t *, uint8_t); //!< LoRa receive buffer read method.
} LoRa_t;

/** @} */
#endif
