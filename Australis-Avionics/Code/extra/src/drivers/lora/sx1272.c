/**************************************************************************************************
 * @file        sx1272.c                                                                          *
 * @author      Matt Ricci                                                                        *
 * @addtogroup  LoRa                                                                              *
 * @brief       Brief description of the file's purpose.                                          *
 *                                                                                                *
 * @todo Implement adjustable packet size                                                         *
 * @{                                                                                             *
 **************************************************************************************************/

#include "sx1272.h"

/* ============================================================================================== */
/**
 * @brief  Initializes the LoRa module with specified configuration parameters.
 *
 * @param  *lora Pointer to LoRa struct to be initialised.
 * @param  *spi  Pointer to SPI peripheral struct.
 * @param  cs    Device chip select GPIO.
 * @param  bw    Bandwidth setting for the LoRa module.
 * @param  sf    Spreading factor for the LoRa module.
 * @param  cr    Coding rate for the LoRa module.
 *
 * @return Ininitialised SX1272 device struct.
 **
 * ============================================================================================== */
SX1272_t SX1272_init(
    SX1272_t *lora,
    SPI_t *spi,
    GPIOpin_t cs,
    SX1272_Bandwidth bw,
    SX1272_SpreadingFactor sf,
    SX1272_CodingRate cr
) {
  lora->base         = spi;
  lora->cs           = cs;
  lora->standby      = SX1272_standby;
  lora->enableBoost  = SX1272_enableBoost;
  lora->transmit     = SX1272_transmit;
  lora->startReceive = SX1272_startReceive;
  lora->readReceive  = SX1272_readReceive;
  lora->clearIRQ     = SX1272_clearIRQ;

  // Set mode to sleep
  _SX1272_setMode(lora, SX1272_MODE_SLEEP);

  /* clang-format off */
  SX1272_writeRegister(lora, SX1272_REG_OP_MODE, 
     0x01 << SX1272_OP_MODE_LONG_RANGE_Pos  // Enable LoRa
  ); 

  SX1272_writeRegister(lora, SX1272_REG_MODEM_CONFIG1, 
    bw   << SX1272_REG_MODEM_CONFIG1_BW_Pos     // Set bandwidth
  | cr   << SX1272_REG_MODEM_CONFIG1_CR_Pos     // Set coding rate
  | 0x00 << SX1272_REG_MODEM_CONFIG1_CRC_Pos    // Enable CRC
  );
  /* clang-format on */

  // TODO: make this configurable in driver
  //
  // Set spreading factor
  SX1272_writeRegister(lora, SX1272_REG_MODEM_CONFIG2, 0x94);

  // Set payload length
  SX1272_writeRegister(lora, SX1272_REG_PAYLOAD_LENGTH, LORA_MSG_LENGTH);
  SX1272_writeRegister(lora, SX1272_REG_MAX_PAYLOAD_LENGTH, LORA_MSG_LENGTH);

  // TODO: make this configurable in driver
  //
  // Set FIFO base addresses
  SX1272_writeRegister(lora, SX1272_REG_FIFO_TX_BASE_ADDR, 0x00); // Tx starts at 0x00
  SX1272_writeRegister(lora, SX1272_REG_FIFO_RX_BASE_ADDR, 0x00); // Rx starts at 0x00

  // Set mode to standby
  _SX1272_setMode(lora, SX1272_MODE_STDBY);

  return *lora;
}

#ifndef DOXYGEN_PRIVATE

/**************************************** PRIVATE METHODS *****************************************/

/* ============================================================================================== */
/**
 * @brief  Sets the operational mode of the LoRa module.
 *
 * @param  *lora        Pointer to LoRa struct.
 * @param  mode         Desired operational mode to be set.
 * @return @c NULL.
 **
 * ============================================================================================== */
void _SX1272_setMode(SX1272_t *lora, SX1272_Mode mode) {
  uint8_t regOpMode = SX1272_readRegister(lora, SX1272_REG_OP_MODE);
  regOpMode &= ~0x07; // Mask to mode bits
  regOpMode |= mode;  // Set mode
  SX1272_writeRegister(lora, SX1272_REG_OP_MODE, regOpMode);
}

#endif

/***************************************** PUBLIC METHODS *****************************************/

/* ============================================================================================== */
/**
 * @brief  Enables/disables power amplifier boost
 *
 * @param  *lora  Pointer to LoRa struct.
 * @param  enable Boolean value for the enable toggle.
 *
 * @return @c NULL.
 **
 * ============================================================================================== */
void SX1272_enableBoost(SX1272_t *lora, bool enable) {
  uint8_t regPaConfig  = SX1272_readRegister(lora, SX1272_REG_PA_CONFIG); // Read current config
  regPaConfig         &= ~SX1272_PA_SELECT;                               // Mask out PA select bit
  SX1272_writeRegister(lora, SX1272_REG_PA_CONFIG, regPaConfig | SX1272_PA_SELECT);
}

/* ============================================================================================== */
/**
 * @brief  Sets the operational mode of the LoRa module to standby.
 *
 * @param  *lora Pointer to LoRa struct.
 *
 * @return @c NULL.
 **
 * ============================================================================================== */
void SX1272_standby(SX1272_t *lora) {
  _SX1272_setMode(lora, SX1272_MODE_STDBY);
}

/* ============================================================================================== */
/**
 * @brief Transmits data using the SX1272.
 *
 * @param lora         Pointer to SX1272 struct.
 * @param pointerdata  Pointer to the data to be transmitted.
 **
 * ============================================================================================== */
void SX1272_transmit(SX1272_t *lora, uint8_t *pointerdata) {
  // Set device to standby
  _SX1272_setMode(lora, SX1272_MODE_STDBY);

  // TODO: add in proper read-mask-write operation for setting DIO mapping
  //
  // Set DIO interrupt pin to TxDone
  SX1272_writeRegister(lora, SX1272_REG_DIO_MAPPING1, SX1272_LORA_DIO_TXDONE);

  // Since the device will only ever be transmitting or receiving at any given time
  // and each packet should be handled immediately by the implementation (no waiting
  // on buffering), we don't need to be concerned about the buffer being overwritten.
  //
  // ...for now.

  // TODO:
  // Think of a more elegant solution for applications that might use this
  // driver that want buffered data
  //
  // Clear IRQ flags and set FIFO address pointer.
  SX1272_writeRegister(lora, SX1272_REG_IRQ_FLAGS, SX1272_LORA_IRQ_TXDONE); // clears the IRQ flag
  SX1272_writeRegister(lora, SX1272_REG_FIFO_ADDR_PTR, 0x00);               // set pointer adddress to start

  // Load data into transmit FIFO
  for (int i = 0; i < 32; i++) {
    SX1272_writeRegister(lora, SX1272_REG_FIFO, pointerdata[i]);
  }

  // Update the current operating mode
  lora->currentMode = SX1272_MODE_TX;       // Set local mode setting
  _SX1272_setMode(lora, lora->currentMode); // Start transmitting
}

/* ============================================================================================== */
/**
 * @brief Begins continuous receive on the SX1272.
 *
 * @param lora Pointer to SX1272 struct.
 **
 * ============================================================================================== */
void SX1272_startReceive(SX1272_t *lora) {
  // Set device to standby
  _SX1272_setMode(lora, SX1272_MODE_STDBY);

  // TODO: add in proper read-mask-write operation for setting DIO mapping
  //
  // Set DIO interrupt pin to RxDone
  SX1272_writeRegister(lora, SX1272_REG_DIO_MAPPING1, SX1272_LORA_DIO_RXDONE);

  // Since the device will only ever be transmitting or receiving at any given time
  // and each packet should be handled immediately by the implementation (no waiting
  // on buffering), we don't need to be concerned about the buffer being overwritten.
  //
  // ...for now.

  // TODO:
  // Think of a more elegant solution for applications that might use this
  // driver that want buffered data
  //
  // Clear IRQ flags and set FIFO address pointer.
  SX1272_writeRegister(lora, SX1272_REG_IRQ_FLAGS, SX1272_LORA_IRQ_RXDONE); // Clear the IRQ flag
  SX1272_writeRegister(lora, SX1272_REG_FIFO_ADDR_PTR, 0x00);               // Set pointer adddress to start

  // Update the current operating mode
  lora->currentMode = SX1272_MODE_RXCONTINUOUS; // Set local mode setting
  _SX1272_setMode(lora, lora->currentMode);     // Start receiving
}

/* ============================================================================================== */
/**
 * @brief  Reads contents of received packet to local buffer from the SX1272.
 *
 * @param  lora     Pointer to SX1272 struct.
 * @param  buffer   Pointer to the buffer to store received data.
 * @param  buffSize Integer representing the size of the buffer to to fill.
 *
 * @return Boolean value indicating if a packet was successfully received and
 *         returned in buffer.
 **
 * ============================================================================================== */
bool SX1272_readReceive(SX1272_t *lora, uint8_t *buffer, uint8_t buffSize) {

  // TODO: Error handling for IRQ flags
  //
  // Currently the readReceive() method clears the SX1272 RxDone IRQ flag before
  // starting the read. This is fine for cases where the user code carefully
  // manages the DIO interrupts, however ideally the method should check for
  // errors in the IRQ register and appropriately discard received packets.

  // Clear the IRQ flag
  SX1272_writeRegister(lora, SX1272_REG_IRQ_FLAGS, SX1272_LORA_IRQ_RXDONE);

  // Read address and packet width information of received data
  uint8_t bytesReceived = SX1272_readRegister(lora, SX1272_REG_RX_BYTES);          // Number of bytes received
  uint8_t rxCurrentAddr = SX1272_readRegister(lora, SX1272_REG_FIFO_RX_CURR_ADDR); // Address of last packet

  // Return error if buffer is smaller than the received data
  if (bytesReceived > buffSize)
    return false;

  // Otherwise, set the address pointer and read each byte into buffer
  SX1272_writeRegister(lora, SX1272_REG_FIFO_ADDR_PTR, rxCurrentAddr);
  for (int i = 0; i < bytesReceived; i++) {
    buffer[i] = SX1272_readRegister(lora, SX1272_REG_FIFO);
  }

  return true;
}

/* ============================================================================================== */
/**
 * @brief  Sets the value of RegIrqFlags in the SX1272 to the provided argument value.
 *         Writing a 1 to a bit in the register will clear the associated flag.
 *
 * @param  lora  Pointer to SX1272 struct.
 * @param  flags 8-bit value representing flag bits to be set.
 *
 * @return @c NULL
 **
 * ============================================================================================== */
void SX1272_clearIRQ(SX1272_t *lora, uint8_t flags) {
  SX1272_writeRegister(lora, SX1272_REG_IRQ_FLAGS, flags);
}

/*************************************** INTERFACE METHODS ****************************************/

void SX1272_writeRegister(SX1272_t *lora, uint8_t address, uint8_t data) {
  SPI_t *spi   = lora->base;
  GPIOpin_t cs = lora->cs;

  // Pull CS low
  cs.reset(&cs);

  // Send write data and address
  uint8_t payload = address | 0x80; // Load payload with address and write command
  spi->transmit(spi, payload);      // Transmit payload
  spi->transmit(spi, data);         // Transmit write data

  // Set CS high
  cs.set(&cs);
}

uint8_t SX1272_readRegister(SX1272_t *lora, uint8_t address) {
  uint8_t response = 0;
  SPI_t *spi       = lora->base;
  GPIOpin_t cs     = lora->cs;

  // Pull CS low
  cs.reset(&cs);

  // Send write data and address
  uint8_t payload = address & 0x7F;              // Load payload with address and read command
  response        = spi->transmit(spi, payload); // Transmit payload
  response        = spi->transmit(spi, 0xFF);    // Transmit dummy data and reasd response

  // Set CS high
  cs.set(&cs);

  return response;
}

/** @} */
