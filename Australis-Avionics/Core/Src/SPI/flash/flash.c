/***********************************************************************************
 * @file        flash.c                                                            *
 * @author      Matt Ricci                                                         *
 * @addtogroup  Flash                                                              *
 * @brief       Brief description of the file's purpose.                           *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "flash.h"

/* SPI4 FLASH
 * ---------------------------------------------
 * Flash Pin       | MCU GPIO Pin  | SIGNAL TYPE
 * ----------------|---------------|------------
 * SDI             | PE14          | DATA
 * SDO             | PE13          | DATA
 * SCLK            | PE12          | DATA
 * CS              | PE11          | CONTROL
 * Memory Hold     | PE10          | CONTROL
 * Write Protect   | PE9           | CONTROL    */

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
DeviceHandle_t Flash_init(
    Flash *flash,
    char name[DEVICE_NAME_LENGTH],
    GPIO_TypeDef *port,
    unsigned long cs,
    int pageSize,
    long pageCount
) {
  SPI_init(&flash->base, MEMORY_FLASH, SPI4, MODE16, port, cs);
  flash->pageSize  = pageSize;
  flash->pageCount = pageCount;
  flash->erase     = Flash_erase;
  flash->readPage  = Flash_readPage;
  flash->writePage = Flash_writePage;

  DeviceHandle_t handle;
  strcpy(handle.name, name);
  handle.device = flash;
  return handle;
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
void _Flash_writeEnable(Flash *flash) {
  SPI spi = flash->base;

  spi.port->ODR &= ~spi.cs;
  spi.transmit(&spi, FLASH_WRITE_ENABLE);
  spi.port->ODR |= spi.cs;
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
void _Flash_readStatus1(Flash *flash, uint8_t *status) {
  SPI spi = flash->base;

  spi.port->ODR &= ~spi.cs;
  *status = spi.transmit(&spi, FLASH_READ_STATUS_REGISTER_1);
  *status = spi.transmit(&spi, 0x0F);
  spi.port->ODR |= spi.cs;
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
void _Flash_readStatus2(Flash *flash, uint8_t *status) {
  SPI spi = flash->base;

  spi.port->ODR &= ~spi.cs;
  *status = spi.transmit(&spi, FLASH_READ_STATUS_REGISTER_2);
  *status = spi.transmit(&spi, 0x0F);
  spi.port->ODR |= spi.cs;
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
void _Flash_readStatus3(Flash *flash, uint8_t *status) {
  SPI spi = flash->base;

  spi.port->ODR &= ~spi.cs;
  *status = spi.transmit(&spi, FLASH_READ_STATUS_REGISTER_3);
  *status = spi.transmit(&spi, 0x0F);
  spi.port->ODR |= spi.cs;
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
void Flash_erase(Flash *flash) {
  _Flash_writeEnable(flash);
  SPI spi        = flash->base;
  uint8_t status = 0;

  // Send Erase Chip instruction
  spi.port->ODR &= ~spi.cs;
  spi.transmit(&spi, FLASH_ERASE_CHIP);
  spi.port->ODR |= spi.cs;

  // Wait until chip BUSY is clear
  do {
    _Flash_readStatus1(flash, &status);
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
void Flash_writePage(Flash *flash, uint32_t address, uint8_t *data) {
  _Flash_writeEnable(flash);
  uint8_t status = 0;
  SPI spi        = flash->base;

  spi.port->ODR &= ~spi.cs;

  // Send Page Program instruction and 24-bit address
  spi.transmit(&spi, FLASH_PAGE_PROGRAM);
  spi.transmit(&spi, (address & 0xFF0000) >> 16);
  spi.transmit(&spi, (address & 0xFF00) >> 8);
  spi.transmit(&spi, (address & 0xFF));

  // Send page data
  for (int i = 0; i < 256; i++) {
    spi.transmit(&spi, data[i]);
  }

  spi.port->ODR |= spi.cs;

  // Wait until chip BUSY is clear
  do {
    _Flash_readStatus1(flash, &status);
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
void Flash_readPage(Flash *flash, uint32_t address, volatile uint8_t *data) {
  SPI spi = flash->base;

  spi.port->ODR &= ~spi.cs;

  // Send Read Data instruction and 24-bit address
  spi.transmit(&spi, FLASH_READ_DATA);
  spi.transmit(&spi, (address & 0xFF0000) >> 16);
  spi.transmit(&spi, (address & 0xFF00) >> 8);
  spi.transmit(&spi, (address & 0xFF));

  for (int i = 0; i < 256; i++) {
    data[i] = spi.transmit(&spi, 0x0F);
  }

  spi.port->ODR |= spi.cs;
}

/** @} */
