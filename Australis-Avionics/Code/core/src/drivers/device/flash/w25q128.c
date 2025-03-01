/***********************************************************************************
 * @file        flash.c                                                            *
 * @author      Matt Ricci                                                         *
 * @addtogroup  Flash                                                              *
 * @brief       Brief description of the file's purpose.                           *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "w25q128.h"
#include "string.h"

/* =============================================================================== */
/**
 * @brief Initialise flash struct.
 *
 * @param *flash	Pointer to Flash struct.
 * @param *port   Pointer to GPIO port.
 * @param cs      Address to flash chip select.
 * @return @c NULL.
 **
 * =============================================================================== */
W25Q128_t W25Q128_init(
    W25Q128_t *flash,
    SPI_t *spi,
    GPIOpin_t cs
) {
  flash->base      = spi;
  flash->cs        = cs;
  flash->pageSize  = W25Q128_PAGE_SIZE;
  flash->pageCount = W25Q128_PAGE_COUNT;
  flash->erase     = W25Q128_erase;
  flash->readPage  = W25Q128_readPage;
  flash->writePage = W25Q128_writePage;

  return *flash;
}

/********************************* PRIVATE METHODS *********************************/

#ifndef DOXYGEN_PRIVATE

/* =============================================================================== */
/**
 * @brief Send Write Enable instruction to the flash device.
 *
 * @param *flash			Pointer to Flash struct.
 * @return @c NULL.
 **
 * =============================================================================== */
void _W25Q128_writeEnable(W25Q128_t *flash) {
  SPI_t *spi = flash->base;
  GPIOpin_t cs = flash->cs;

  cs.reset(&cs);
  spi->transmit(spi, W25Q128_WRITE_ENABLE);
  cs.set(&cs);
}

/* =============================================================================== */
/**
 * @brief Read from Status Register 1.
 *
 * @param *flash		Pointer to Flash struct.
 * @param *status 	Pointer to status output variable.
 * @return @c NULL.
 **
 * =============================================================================== */
void _W25Q128_readStatus1(W25Q128_t *flash, uint8_t *status) {
  SPI_t *spi = flash->base;
  GPIOpin_t cs = flash->cs;

  cs.reset(&cs);
  *status = spi->transmit(spi, W25Q128_READ_STATUS_REGISTER_1);
  *status = spi->transmit(spi, 0x0F);
  cs.set(&cs);
}

/* =============================================================================== */
/**
 * @brief Read from Status Register 2.
 *
 * @param *flash		Pointer to Flash struct.
 * @param *status 	Pointer to status output variable.
 * @return @c NULL.
 **
 * =============================================================================== */
void _W25Q128_readStatus2(W25Q128_t *flash, uint8_t *status) {
  SPI_t *spi = flash->base;
  GPIOpin_t cs = flash->cs;

  cs.reset(&cs);
  *status = spi->transmit(spi, W25Q128_READ_STATUS_REGISTER_2);
  *status = spi->transmit(spi, 0x0F);
  cs.set(&cs);
}

/* =============================================================================== */
/**
 * @brief Read from Status Register 3.
 *
 * @param *flash		Pointer to Flash struct.
 * @param *status 	Pointer to status output variable.
 * @return @c NULL.
 **
 * =============================================================================== */
void _W25Q128_readStatus3(W25Q128_t *flash, uint8_t *status) {
  SPI_t *spi = flash->base;
  GPIOpin_t cs = flash->cs;

  cs.reset(&cs);
  *status = spi->transmit(spi, W25Q128_READ_STATUS_REGISTER_3);
  *status = spi->transmit(spi, 0x0F);
  cs.set(&cs);
}

#endif

/********************************* DEVICE METHODS **********************************/

/* =============================================================================== */
/**
 * @brief Erase flash chip.
 *
 * @param *flash		Pointer to Flash struct.
 * @return @c NULL.
 **
 * =============================================================================== */
void W25Q128_erase(W25Q128_t *flash) {
  _W25Q128_writeEnable(flash);
  SPI_t *spi     = flash->base;
  GPIOpin_t cs   = flash->cs;

  uint8_t status = 0;

  // Send Erase Chip instruction
  cs.reset(&cs);
  spi->transmit(spi, W25Q128_ERASE_CHIP);
  cs.set(&cs);

  // Wait until chip BUSY is clear
  do {
    _W25Q128_readStatus1(flash, &status);
  } while (status & 0x01);
}

/* =============================================================================== */
/**
 * @brief Write page to flash.
 *
 * @param *flash		Pointer to Flash struct.
 * @param address 	Address in memory to write to.
 * @param *data 		Pointer to start of page buffer to write.
 * @return @c NULL.
 **
 * =============================================================================== */
void W25Q128_writePage(W25Q128_t *flash, uint32_t address, uint8_t *data) {
  _W25Q128_writeEnable(flash);
  SPI_t *spi     = flash->base;
  GPIOpin_t cs   = flash->cs;

  uint8_t status = 0;

  cs.reset(&cs);

  // Send Page Program instruction and 24-bit address
  spi->transmit(spi, W25Q128_PAGE_PROGRAM);
  spi->transmit(spi, (address & 0xFF0000) >> 16);
  spi->transmit(spi, (address & 0xFF00) >> 8);
  spi->transmit(spi, (address & 0xFF));

  // Send page data
  for (int i = 0; i < 256; i++) {
    spi->transmit(spi, data[i]);
  }

  cs.set(&cs);

  // Wait until chip BUSY is clear
  do {
    _W25Q128_readStatus1(flash, &status);
  } while (status & 0x1);
}

/* =============================================================================== */
/**
 * @brief Read from flash.
 *
 * @param *flash		Pointer to Flash struct.
 * @param address 	Address in memory to write to.
 * @param *data 		Pointer to start of page buffer to read to.
 * @return @c NULL.
 **
 * =============================================================================== */
void W25Q128_readPage(W25Q128_t *flash, uint32_t address, volatile uint8_t *data) {
  SPI_t *spi   = flash->base;
  GPIOpin_t cs = flash->cs;

  cs.reset(&cs);

  // Send Read Data instruction and 24-bit address
  spi->transmit(spi, W25Q128_READ_DATA);
  spi->transmit(spi, (address & 0xFF0000) >> 16);
  spi->transmit(spi, (address & 0xFF00) >> 8);
  spi->transmit(spi, (address & 0xFF));

  for (int i = 0; i < 256; i++) {
    data[i] = spi->transmit(spi, 0x0F);
  }

  cs.set(&cs);
}

/** @} */
