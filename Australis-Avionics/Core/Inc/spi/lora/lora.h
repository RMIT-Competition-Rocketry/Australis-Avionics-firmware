/**
 * @author Matt Ricci
 * @ingroup SPI
 * @addtogroup LoRa
 */

#ifndef _LORA_H
#define _LORA_H

#include "stm32f439xx.h"
#include "spi.h"

#define LORA_REG_FIFO                   0x00
#define LORA_REG_FIFO_ADDR_PTR          0x0D
#define LORA_REG_FIFO_TX_BASE_ADDR      0x0E
#define LORA_REG_FIFO_RX_BASE_ADDR      0x0F
#define LORA_REG_FIFO_RX_CURR_ADDR      0x10
#define LORA_REG_IRQ_FLAGS_MASK         0x11
#define LORA_REG_IRQ_FLAGS              0x12

#define LORA_REG_OP_MODE                0x01
#define LORA_REG_OP_MODE_LONG_RANGE_Pos 0x07
#define LORA_REG_OP_MODE_MODE_Pos       0x00

#define LORA_REG_MODEM_CONFIG1          0x1D
#define LORA_REG_MODEM_CONFIG1_BW_Pos   0x06
#define LORA_REG_MODEM_CONFIG1_CR_Pos   0x03
#define LORA_REG_MODEM_CONFIG1_CRC_Pos  0x01

#define LORA_REG_MODEM_CONFIG2          0x1E
#define LORA_REG_MODEM_CONFIG2_SF_Pos   0x04

#define LORA_REG_PAYLOAD_LENGTH         0x22
#define LORA_REG_MAX_PAYLOAD_LENGTH     0x23

#define RegSymbTimeoutLsb               0x1F
#define RegPreambleMsb                  0x20
#define RegPreambleLsb                  0x21
#define RegHopPeriod                    0x24
#define RegFifoRxByteAddr               0x25
#define RegDioMapping1                  0x40
#define RegDioMapping2                  0x41

#define LORA_MSG_LENGTH                 0x20
#define LORA_MSG_PAYLOAD_LENGTH         (LORA_MSG_LENGTH - 1)

/**
 * @ingroup LoRa
 * @addtogroup SX1272
 * @{
 */

typedef enum {
  BW125,
  BW250,
  BW500,
} Bandwidth;

typedef enum {
  CR5 = 1,
  CR6,
  CR7,
  CR8,
} CodingRate;

typedef enum {
  SF6 = 6,
  SF7,
  SF8,
  SF9,
  SF10,
  SF11,
  SF12,
} SpreadingFactor;

typedef enum {
  SLEEP,
  STDBY,
  FSTX,
  TX,
  FSRX,
  RXCONTINUOUS,
  RXSINGLE,
  CAD
} Mode;

typedef struct {
  uint8_t id;                            //!< Packet header ID
  uint8_t data[LORA_MSG_PAYLOAD_LENGTH]; //!< Packet payload
} SX1272_Packet;

/** @extends SPI */
typedef struct LoRa {
  SPI base;                                   //!< Parent SPI interface
  void (*transmit)(struct LoRa *, uint8_t *); //!< LoRa transmit method. @see SX1272_transmit
} SX1272_t;

SX1272_t SX1272_init(SX1272_t *, GPIO_TypeDef *, unsigned long, Bandwidth, SpreadingFactor, CodingRate);
void SX1272_transmit(SX1272_t *, uint8_t *);
void SX1272_writeRegister(SX1272_t *, uint8_t, uint8_t);
uint8_t SX1272_readRegister(SX1272_t *, uint8_t);

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

void _SX1272_setMode(SX1272_t *, Mode);

/** @} */
#endif
