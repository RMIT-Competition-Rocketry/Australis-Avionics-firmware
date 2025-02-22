/***********************************************************************************
 * @file        spi.c                                                              *
 * @author      Matt Ricci                                                         *
 * @addtogroup  SPI                                                                *
 * @brief       Implements functions for SPI device interface initialization and   *
 *              communication.                                                     *
 *                                                                                 *
 * @todo        Rewrite initialisation to implement enum check for 8/16 bit mode   *
 * 							and assign approprriate function pointers 												 *
 ***********************************************************************************/

#include "spi.h"

#include "stddef.h"

static void SPI_send8(SPI_t *, uint16_t);
static void SPI_send16(SPI_t *, uint16_t);

static void SPI_receive8(SPI_t *, volatile uint16_t *);
static void SPI_receive16(SPI_t *, volatile uint16_t *);

static void _SPI_init(SPI_TypeDef *, SPI_Config *);

/* =============================================================================== */
/**
 * @brief Initialiser for an SPI device interface.
 *
 * @param *spi 				Pointer to SPI struct to be initialised.
 * @param device 			Enum specifier for device type.
 * @param *interface 	Pointer to SPI interface struct.
 * @param *port 			Pointer to GPIO port struct.
 * @param cs 					Device chip select address.
 * @return @c NULL.
 **
 * =============================================================================== */
SPI_t SPI_init(SPI_TypeDef *peripheral, SPI_Config *config) {
  // Early return error struct if peripheral is NULL
  if (peripheral == NULL)
    return (SPI_t){.interface = NULL};

  // Create SPI struct from parameters and initialise methods
  SPI_t spi;
  spi.send         = (config->DFF == SPI_DFF8) ? SPI_send8 : SPI_send16;
  spi.receive      = (config->DFF == SPI_DFF8) ? SPI_receive8 : SPI_receive16;
  spi.transmit     = SPI_transmit;
  spi.updateConfig = SPI_updateConfig;

  // Update config and enable peripheral
  SPI_updateConfig(&spi, config);

  return spi;
}

// ALLOW FORMATTING
#ifndef DOXYGEN_PRIVATE

/* =============================================================================== */
/**
 * @brief   Private initialiser for SPI registers.
 * @details
 *
 * @param   interface Pointer to the SPI_TypeDef struct representing the pin's port.
 * @param   config    Pointer to SPI_Config struct for initial configuration.
 *                    This may be passed as \c NULL to initialise a default
 *                    configuration. @see SPI_Config
 *
 * @return  @c NULL.
 **
 * =============================================================================== */
static void _SPI_init(SPI_TypeDef *interface, SPI_Config *config) {
  // Wait until interface is not busy
  while (interface->SR & SPI_SR_BSY);

  // Disable interface and update configuration
  config->SPE    = false;
  interface->CR1 = *(uint32_t *)config;

  // Re-enable the interface
  config->SPE    = true;
  interface->CR1 = *(uint32_t *)config;
}

#endif

/* =============================================================================== */
/**
 * @brief Instance method to communicate a SPI transaction with slave device.
 *
 * @param 	*spi 			Pointer to SPI struct.
 * @param 	data 			Data payload to be sent to slave device.
 * @retval 	response 	Returns the slave device response from the transaction.
 **
 * =============================================================================== */
uint16_t SPI_transmit(SPI_t *spi, uint16_t data) {
  volatile uint16_t response;
  spi->send(spi, data);
  spi->receive(spi, &response);
  while (spi->interface->SR & SPI_SR_BSY);
  return response;
}

/* =============================================================================== */
/**
 * @brief Send data through the SPI interface.
 *
 * @param 	*spi 			Pointer to SPI struct.
 * @param   data      The data to send.
 * @return @c NULL.
 **
 * =============================================================================== */
static void SPI_send8(SPI_t *spi, uint16_t data) {
  while (!(spi->interface->SR & SPI_SR_TXE));
  spi->interface->DR = (uint8_t)data;
}

static void SPI_send16(SPI_t *spi, uint16_t data) {
  while (!(spi->interface->SR & SPI_SR_TXE));
  spi->interface->DR = data;
}

/* =============================================================================== */
/**
 * @brief Receive data through the SPI interface.
 *
 * @param 	*spi 			Pointer to SPI struct.
 * @param   data      Pointer to variable to receive data into.
 * @return @c NULL.
 **
 * =============================================================================== */
static void SPI_receive8(SPI_t *spi, volatile uint16_t *data) {
  while (!(spi->interface->SR & SPI_SR_RXNE));
  *data = (uint8_t)spi->interface->DR;
}

static void SPI_receive16(SPI_t *spi, volatile uint16_t *data) {
  while (!(spi->interface->SR & SPI_SR_RXNE));
  *data = spi->interface->DR;
}

/* =============================================================================== */
/**
 * @brief   Update SPI peripheral configuration
 * @details Uses the provided configuration to update the SPI registers and resets the
 *          associated peripheral.
 *          As with initialisation, passing \c NULL will set the default config.
 *
 * @param   spi Pointer to SPI_t struct.
 *
 * @return  @c NULL.
 **
 * =============================================================================== */
void SPI_updateConfig(SPI_t *spi, SPI_Config *config) {
  // Initialise config with default values if passed NULL.
  if (config == NULL) {
    config = &SPI_CONFIG_DEFAULT;
  }

  // Update peripheral with new config
  spi->config = *config;

  // Initialise SPI registers and enable peripheral
  _SPI_init(spi->interface, config);
}
