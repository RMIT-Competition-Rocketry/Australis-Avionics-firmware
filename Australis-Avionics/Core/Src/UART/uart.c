/***********************************************************************************
 * @file        uart.c                                                             *
 * @author      Matt Ricci                                                         *
 * @addtogroup  UART                                                               *
 * @brief       Brief description of the file's purpose.                           *
 *                                                                                 *
 * @todo Tidy up `_UART_setup`                                                     *
 * @todo Implement printf                                                          *
 * @todo Add println function                                                      *
 ***********************************************************************************/

#include "uart.h"

/* =============================================================================== */
/**
 * @brief Initialiser for a UART device interface.
 *
 * @param *uart 			Pointer to UART struct to be initialised.
 * @param *interface 	Pointer to UART interface struct.
 * @param *port 			Pointer to GPIO port struct.
 * @param baud 				UART baud rate.
 * @param over8 			Oversampling mode.
 * @return @c NULL.
 **
 * =============================================================================== */
DeviceHandle_t UART_init(
    UART *uart,
    char name[DEVICE_NAME_LENGTH],
    USART_TypeDef *interface,
    GPIO_TypeDef *port,
		UART_Pins pins,
    uint32_t baud,
    OversampleMode over8
) {
	uart->setBaud   = UART_setBaud;
  uart->send      = UART_send;
  uart->sendBytes = UART_sendBytes;
  uart->print     = UART_print;
  uart->receive   = UART_receive;
  uart->interface = interface;
  uart->port      = port;
  uart->pins      = pins;
  uart->baud      = baud;
  uart->over8     = over8;

  _UART_setup(uart, pins);
  DeviceHandle_t handle;
  strcpy(handle.name, name);
  handle.device = uart;

  return handle;
}

/********************************** PRIVATE METHODS *********************************/

#ifndef DOXYGEN_PRIVATE

/* =============================================================================== */
/**
 * @brief Configures the UART interface for communication.
 *
 * @param *uart Pointer to UART struct containing configuration parameters.
 * @return @c NULL.
 **
 * =============================================================================== */
void _UART_setup(UART *uart, UART_Pins pins) {
  GPIO_TypeDef *port       = uart->port;
  USART_TypeDef *interface = uart->interface;

	// Clear MODER bits for the TX and RX pins
  port->MODER &= ~((0x03 << GPIO_MODER(pins.TX)) | (0x03 << GPIO_MODER(pins.RX))); 							// Clear mode bits
  port->MODER |= (GPIO_MODE_AF << GPIO_MODER(pins.TX)) | (GPIO_MODE_AF << GPIO_MODER(pins.RX)); // Set mode to AF

  // Clear AFR for the TX and RX pins and set AF8
  port->AFR[pins.TX / 8] &= ~(0x0F << ((pins.TX % 8) * 4)); 	 // Clear AF bits for TX
  port->AFR[pins.RX / 8] &= ~(0x0F << ((pins.RX % 8) * 4)); 	 // Clear AF bits for RX
  port->AFR[pins.TX / 8] |= ((interface <= USART3 ? UART_AF7 : UART_AF8) << ((pins.TX % 8) * 4)); // Set AF8 for TX
  port->AFR[pins.RX / 8] |= ((interface <= USART3 ? UART_AF7 : UART_AF8) << ((pins.RX % 8) * 4)); // Set AF8 for RX

  // Set pull-up for RX pin
  port->PUPDR &= ~(0x03 << GPIO_PUPDR(pins.RX)); 							 // Clear pull-up/pull-down bits for RX
  port->PUPDR |= (GPIO_PULL_UP << GPIO_PUPDR(pins.TX));				 // Set pull-up for RX

  // Set speed for TX and RX pins
  port->OSPEEDR &= ~((0x03 << GPIO_OSPEEDR(pins.TX)) | (0x03 << GPIO_OSPEEDR(pins.RX))); // Clear speed bits
  port->OSPEEDR |= (GPIO_SPEED_HIGH << GPIO_OSPEEDR(pins.TX)); 						 			 				 // Set high speed for TX
  port->OSPEEDR |= (GPIO_SPEED_HIGH << GPIO_OSPEEDR(pins.RX)); 						 			 				 // Set high speed for RX

  // Calculate USARTDIV
  uint16_t usartDiv = 168000000 / ((2 - (uart->over8)) * uart->baud);
  interface->BRR &= 0xFFFF0000; // Clear mantissa and div in baud rate reg
  interface->BRR |= usartDiv; 	// Set baud rate

  interface->CR1 &= ~USART_CR1_PCE;     													// disable parity
  interface->CR2 &= ~USART_CR2_CLKEN;   													// disable synchrnous mode
  interface->CR3 &= ~(USART_CR3_CTSE | USART_CR3_RTSE);   				// disable flow control
	interface->CR1 |= (USART_CR1_RXNEIE); 													// enable RXNE interrupt
  interface->CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_TE); // enable usart, enable receive and transmit
}

#endif

/********************************** INTERFACE METHODS ********************************/

void UART_setBaud(UART *uart, uint32_t baud) {
  GPIO_TypeDef *port       = uart->port;
  USART_TypeDef *interface = uart->interface;	
	
	USART1->CR1 &= ~USART_CR1_UE;

	// Calculate USARTDIV
  uint16_t usartDiv = 168000000 / ((2 - (uart->over8)) * baud);
  interface->BRR &= 0xFFFF0000; // Clear mantissa and div in baud rate reg
  interface->BRR |= usartDiv; 	// Set baud rate
	
	USART1->CR1 |= USART_CR1_UE;
}

/* =============================================================================== */
/**
 * @brief Sends a single byte of data over the UART interface.
 *
 * @param *uart  Pointer to UART struct.
 * @param data   Byte of data to be sent.
 * @return @c NULL.
 **
 * =============================================================================== */
void UART_send(UART *uart, uint8_t data) {
  USART_TypeDef *interface = uart->interface;
  while ((interface->SR & USART_SR_TXE) == 0);  
  interface->DR = data;
  while ((interface->SR & USART_SR_TC) == 0);
}

/* =============================================================================== */
/**
 * @brief Sends an array of bytes over the UART interface.
 *
 * @param *uart   Pointer to UART struct.
 * @param *data   Pointer to the array of bytes to be sent.
 * @param length  Number of bytes to send.
 * @return @c NULL.
 **
 * =============================================================================== */
void UART_sendBytes(UART *uart, uint8_t *data, int length) {
  for (int i = 0; i < length; i++)
    UART_send(uart, data[i]);
}

/* =============================================================================== */
/**
 * @brief Sends a string of characters over the UART interface.
 *
 * @param *uart  Pointer to UART struct.
 * @param *data  Pointer to the string of characters to be sent.
 * @return @c NULL.
 **
 * =============================================================================== */

void UART_print(UART *uart, char *data) {
  int i = 0;
  while (data[i] != '\0')
    UART_send(uart, data[i++]);
}

/* =============================================================================== */
/**
 * @brief Receives a single byte of data from the UART interface.
 *
 * @param *uart  Pointer to UART struct.
 * @return       The received byte of data.
 **
 * =============================================================================== */
uint8_t UART_receive(UART *uart) {
  USART_TypeDef *interface = uart->interface;
  while ((interface->SR & USART_SR_RXNE) == 0);
  return (uint8_t)(interface->DR & 0xFF);
}
