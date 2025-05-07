/**
 * @author Matt Ricci
 * @addtogroup API
 * @{
 * @addtogroup UART
 * @}
 */

#ifndef _UART_H
#define _UART_H

#include "stdbool.h"
#include "stm32f439xx.h"
#include "string.h"

#include "gpio.h"

#define UART_PARITY_DISABLE 0x400
#define UART_SYNC_DISABLE   0xE00
#define UART_FLOW_DISABLE   0x300
#define UART_ENABLE         0x2000
#define UART_RX_ENABLE      0x0004
#define UART_TX_ENABLE      0x0008
#define UART_AF7            0x07
#define UART_AF8            0x08

#define SIGINT              0x03
#define BACKSPACE           0x08
#define LINE_FEED           0x0A
#define CARRIAGE_RETURN     0x0D

/**
 * @ingroup UART
 * @addtogroup UART_Interface Interface
 * @brief UART interface from which external peripherals inherit
 * @{
 */

typedef enum {
  OVER16,
  OVER8
} OversampleMode;

typedef struct UART_Pins {
  uint8_t TX;
  uint8_t RX;
} UART_Pins;

/**
 * @brief Struct definition for \ref UART "UART interface"
 */
typedef struct UART {
  USART_TypeDef *interface;
  GPIO_TypeDef *port;
  UART_Pins pins;
  uint32_t baud;
  OversampleMode over8;
  void (*setBaud)(struct UART *, uint32_t);
  void (*send)(struct UART *, uint8_t);             //!< UART send method.   	             @see UART_send
  void (*sendBytes)(struct UART *, uint8_t *, int); //!< UART send multiple bytes method.  @see UART_sendBytes
  void (*print)(struct UART *, char *);             //!< UART print string method.  			 @see UART_print
  uint8_t (*receive)(struct UART *);                //!< UART receive method.              @see UART_receive
} UART_t;

UART_t UART_init(UART_t *, USART_TypeDef *, GPIO_TypeDef *, UART_Pins, uint32_t, OversampleMode);
void _UART_setup(UART_t *, UART_Pins);
void UART_setBaud(UART_t *, uint32_t);

void UART_send(UART_t *, uint8_t data);
void UART_sendBytes(UART_t *, uint8_t *data, int length);
void UART_print(UART_t *, char *data);
uint8_t UART_receive(UART_t *);

/** @} */
#endif
