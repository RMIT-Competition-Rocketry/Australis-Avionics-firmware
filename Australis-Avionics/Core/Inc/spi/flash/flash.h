/**
 * @author Matt Ricci
 * @ingroup SPI
 * @addtogroup Flash
 */

#ifndef _FLASH_H
#define _FLASH_H

#include "stm32f439xx.h"
#include "string.h"

#include "devices.h"
#include "spi.h"

#define FLASH_PAGE_PROGRAM           0x02
#define FLASH_READ_DATA              0x03
#define FLASH_WRITE_ENABLE           0x06
#define FLASH_ERASE_CHIP             0x60
#define FLASH_READ_STATUS_REGISTER_1 0x05
#define FLASH_READ_STATUS_REGISTER_2 0x35
#define FLASH_READ_STATUS_REGISTER_3 0x15

/**
 * @addtogroup Flash
 * @{
 */

/** @extends SPI */
typedef struct Flash {
  SPI base;                                                       //!< Parent SPI interface
  int pageSize;
  long pageCount;
  void (*erase)(struct Flash *);                                  //!< Chip erase method. @see Flash_erase
  void (*readPage)(struct Flash *, uint32_t, volatile uint8_t *); //!< Read page method. 	@see Flash_readPage
  void (*writePage)(struct Flash *, uint32_t, uint8_t *);         //!< Write page method. @see Flash_writePage
} Flash;

void configure_SPI4_Flash();
DeviceHandle_t Flash_init(Flash *, char[DEVICE_NAME_LENGTH], GPIO_TypeDef *, unsigned long, int, long);
void Flash_readPage(Flash *, uint32_t, volatile uint8_t *);
void Flash_writePage(Flash *, uint32_t, uint8_t *);
void Flash_erase(Flash *);
void _Flash_writeEnable(Flash *);
void _Flash_readStatus1(Flash *, uint8_t *);
void _Flash_readStatus2(Flash *, uint8_t *);
void _Flash_readStatus3(Flash *, uint8_t *);

/** @} */
#endif
