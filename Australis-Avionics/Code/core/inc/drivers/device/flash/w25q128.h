/**
 * @author Matt Ricci
 * @ingroup SPI
 * @addtogroup Flash
 */

// ALLOW FORMATTING
#ifndef _FLASH_H
#define _FLASH_H

#include "spi.h"
#include "gpiopin.h"

#define W25Q128_PAGE_PROGRAM           0x02
#define W25Q128_READ_DATA              0x03
#define W25Q128_WRITE_ENABLE           0x06
#define W25Q128_ERASE_CHIP             0x60
#define W25Q128_READ_STATUS_REGISTER_1 0x05
#define W25Q128_READ_STATUS_REGISTER_2 0x35
#define W25Q128_READ_STATUS_REGISTER_3 0x15

#define W25Q128_PAGE_SIZE              256   // 256 bytes per page
#define W25Q128_PAGE_COUNT             65536 // 65536 total pages

/**
 * @ingroup Flash
 * @addtogroup W25Q128
 * @{
 */

typedef struct W25Q128 {
  SPI_t *base;                                                      //!< Parent SPI interface.
  GPIOpin_t cs;                                                     //!< Chip select GPIO.
  int pageSize;                                                     //!< Number of bytes per page.
  long pageCount;                                                   //!< Total number of pages.
  void (*erase)(struct W25Q128 *);                                  //!< Chip erase method. @see W25Q128_erase
  void (*readPage)(struct W25Q128 *, uint32_t, volatile uint8_t *); //!< Read page method. 	@see W25Q128_readPage
  void (*writePage)(struct W25Q128 *, uint32_t, uint8_t *);         //!< Write page method. @see W25Q128_writePage
} W25Q128_t;

void configure_SPI4_Flash();
W25Q128_t W25Q128_init(W25Q128_t *, SPI_t *, GPIOpin_t);
void W25Q128_readPage(W25Q128_t *, uint32_t, volatile uint8_t *);
void W25Q128_writePage(W25Q128_t *, uint32_t, uint8_t *);
void W25Q128_erase(W25Q128_t *);
void _W25Q128_writeEnable(W25Q128_t *);
void _W25Q128_readStatus1(W25Q128_t *, uint8_t *);
void _W25Q128_readStatus2(W25Q128_t *, uint8_t *);
void _W25Q128_readStatus3(W25Q128_t *, uint8_t *);

/** @} */
#endif
