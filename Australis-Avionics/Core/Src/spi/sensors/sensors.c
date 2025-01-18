/***********************************************************************************
 * @file        sensors.c                                                          *
 * @author      Matt Ricci                                                         *
 *                                                                                 *
 * @todo Move initialisation of SPI interface to individual devices initialising   *
 *       relevant registers and peripherals.                                       *
 ***********************************************************************************/

#include "sensors.h"

void configure_SPI1_Sensor_Suite(void) {
  GPIOA->MODER &= (~(GPIO_MODER_MODE5_Msk | GPIO_MODER_MODE6_Msk | GPIO_MODER_MODE7_Msk));
  GPIOA->MODER |= ((0x2 << GPIO_MODER_MODE5_Pos) | (0x2 << GPIO_MODER_MODE6_Pos) | (0x2 << GPIO_MODER_MODE7_Pos));
  GPIOA->AFR[0] &= (~(0xFFF00000));                                                                                        // clears AFRL 5, 6 and 7
  GPIOA->AFR[0] |= (0x55500000);                                                                                           // sets ports 5,6,7 to AF5

  GPIOA->OTYPER &= (~(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7));                                               // configure as push pull
  GPIOA->OSPEEDR &= (~(GPIO_OSPEEDR_OSPEED5_Msk | GPIO_OSPEEDR_OSPEED6_Msk | GPIO_OSPEEDR_OSPEED7_Msk));                   // clears OSPEED
  GPIOA->OSPEEDR |= (0x2 << GPIO_OSPEEDR_OSPEED5_Pos | 0x2 << GPIO_OSPEEDR_OSPEED6_Pos | 0x2 << GPIO_OSPEEDR_OSPEED7_Pos); // sets as high speed
  // pins PC2, PC3, PC4 need to be set to interrupt pins
  //
  //  pins PA2, PA4, PA1, PB0 need to set as general purpose outputs
  GPIOA->MODER &= (~(GPIO_MODER_MODE1_Msk | GPIO_MODER_MODE2_Msk | GPIO_MODER_MODE3_Msk | GPIO_MODER_MODE4_Msk));
  GPIOA->MODER |= ((0x1 << GPIO_MODER_MODE1_Pos) | (0x1 << GPIO_MODER_MODE2_Pos) | (0x1 << GPIO_MODER_MODE3_Pos) | (0x1 << GPIO_MODER_MODE4_Pos));
  GPIOA->OTYPER &= (uint32_t)(~(GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3 | GPIO_OTYPER_OT4));                                                             // sets 0xboth as push-pull
  GPIOA->OSPEEDR &= (~(GPIO_OSPEEDR_OSPEED1_Msk | GPIO_OSPEEDR_OSPEED2_Msk | GPIO_OSPEEDR_OSPEED3_Msk | GPIO_OSPEEDR_OSPEED4_Msk));                                  // clears Port 14 and 15 section
  GPIOA->OSPEEDR |= ((0x2 << GPIO_OSPEEDR_OSPEED1_Pos) | (0x2 << GPIO_OSPEEDR_OSPEED2_Pos) | (0x2 << GPIO_OSPEEDR_OSPEED3_Pos) | (0x2 << GPIO_OSPEEDR_OSPEED4_Pos)); // sets slew rate as high speed
  // PA2 Gryo Chip Select - set to high for default
  //  PA1 Accel 1 Chip Select - set to high for default
  // PA4 Mag - chip select - set to high for dafault

  GPIOA->ODR |= (GPIO_ODR_OD1 | GPIO_ODR_OD2 | GPIO_ODR_OD3 | GPIO_ODR_OD4); // sets to high to disable chip select

  GPIOB->MODER &= (~(GPIO_MODER_MODE0_Msk));
  GPIOB->MODER |= (0x1 << GPIO_MODER_MODE0_Pos);
  GPIOB->OTYPER &= (~(GPIO_OTYPER_OT0));
  GPIOB->OSPEEDR &= (~(GPIO_OSPEEDR_OSPEED0_Msk));
  GPIOB->OSPEEDR |= (0x2 << GPIO_OSPEEDR_OSPEED0_Pos);

  // PB0 Accel 1 Chip Select - set to high for default
  GPIOB->ODR |= (GPIO_ODR_OD0);

  // Clear the First Control register of the SPI peripheral.
  	SPI1->CR1 &= 0xFFFF0000;

  	// Configure the SCLK to be divide by 8,
  	SPI1->CR1 |= (0x02 << SPI_CR1_BR_Pos) | (1 << SPI_CR1_CPOL_Pos) | (1 << SPI_CR1_CPHA_Pos) | (0 << SPI_CR1_DFF_Pos);


  	// Set to full duplex, master mode.
  	// In full duplex, both the MISO and MOSI pins are required.
  	SPI1->CR1 &= ~(SPI_CR1_BIDIMODE);
  	SPI1->CR1 &= ~(SPI_CR1_RXONLY);

  	// Set the slave select - software management.
  	SPI1->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);

  	// Specify master operation.
  	SPI1->CR1 |= SPI_CR1_MSTR;

  	// Manually raise the chip select.
  	GPIOA->ODR |= (1 << GPIO_ODR_OD4_Pos) | (1 << GPIO_ODR_OD3_Pos) | (1 << GPIO_ODR_OD2_Pos) | (1 << GPIO_ODR_OD1_Pos);

  	// Enable the SPI peripheral
  	SPI1->CR1 |= SPI_CR1_SPE;



}
