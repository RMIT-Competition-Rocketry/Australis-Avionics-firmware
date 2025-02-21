/***********************************************************************************
 * @file        gpiopin.c                                                          *
 * @author      Matt Ricci                                                         *
 * @addtogroup  GPIO_Pin GPIO Pin                                                  *
 * @brief       Implements functions for GPIO peripheral pin interface.            *
 *              Includes methods to initialise, set, reset and update the config   *
 *              of an individual GPIO pin.                                         *
 ***********************************************************************************/

#include "gpiopin.h"
#include "stddef.h"
#include "stdlib.h"

/* =============================================================================== */
/**
 * @brief Initialiser for a GPIO peripheral pin interface.
 *
 * @param port   Pointer to the GPIO_TypeDef struct representing the pin's port.
 * @param pin    Enum quantified value of the pin's position in its port.
 * @param config Pointer to GPIO_Config struct for initial configuration.
 *               This may be passed as \c NULL to initialise a default
 *               configuration. @see GPIO_Config
 *
 * @return Initialised GPIOpin_t struct.
 **
 * =============================================================================== */
GPIOpin_t GPIOpin_init(GPIO_TypeDef *port, GPIO_Pin pin, GPIO_Config *config) {
  // Early return error struct if port is NULL
  if (port == NULL)
    return (GPIOpin_t){.port = NULL};

  if (config == NULL) {
    config = &(GPIO_Config){
        GPIO_MODE_OUTPUT,
        GPIO_TYPE_PUSHPULL,
        GPIO_SPEED_HIGH,
        GPIO_PUPD_NONE,
        GPIO_AF0,
    };
  }

  // Get index of supplied port by subtracting GPIOA address and dividing by size
  int portIndex  = ((uint32_t)port - GPIOA_BASE) / GPIO_PERIPHERAL_SIZE;

  RCC->AHB1ENR  |= (0b01 << portIndex);                      // Enable port in RCC
  RCC->AHB1RSTR |= (0b01 << portIndex);                      // Reset port
  __ASM("NOP");                                              //
  __ASM("NOP");                                              //
  RCC->AHB1RSTR &= ~(0b01 << portIndex);                     // Clear reset

  port->MODER   &= ~(0b11 << (2 * pin));                     // Clear MODER bits for pin
  port->MODER   |= (config->mode << (2 * pin));              // Shift in mode bits from config

  port->OTYPER  &= ~(0b01 << pin);                           // Clear OTYPE bits for pin
  port->OTYPER  |= (config->type << pin);                    // Shift in type bits from config

  port->OSPEEDR &= ~(0b11 << (2 * pin));                     // Clear OSPEEDR bits for pin
  port->OSPEEDR |= (config->speed << (2 * pin));             // Shift in speed bits from config

  port->PUPDR   &= ~(0b11 << (2 * pin));                     // Clear PUPDR bits for pin
  port->PUPDR   |= (config->pupd << (2 * pin));              // Shift in pupd bits from config

  uint32_t afr   = (pin <= 7) ? port->AFR[0] : port->AFR[1]; // Select AFRL (pin<=7) or AFRH (pin>7)
  afr           &= ~(0b1111 << (4 * pin));                   // Clear AFR bits for pin
  afr           |= (config->afr << (4 * pin));               // Shift in afr bits from config

  // Create GPIO struct from parameters and initialise methods
  GPIOpin_t GPIO;
  GPIO.config       = *config;
  GPIO.port         = port;
  GPIO.pin          = pin;
  GPIO.set          = GPIOpin_set;
  GPIO.reset        = GPIOpin_reset;
  GPIO.toggle       = GPIOpin_toggle;
  GPIO.updateConfig = GPIOpin_updateConfig;

  // Return the new GPIO struct
  return GPIO;
}

/* =============================================================================== */
/**
 * @brief Initialiser for a
 *
 * @return @c NULL.
 **
 * =============================================================================== */
void GPIOpin_set(GPIOpin_t *gpio) {
  gpio->port->ODR |= (0b01 << gpio->pin);
}

/* =============================================================================== */
/**
 * @brief Initialiser for a
 *
 * @return @c NULL.
 **
 * =============================================================================== */
void GPIOpin_reset(GPIOpin_t *gpio) {
  gpio->port->ODR &= ~(0b01 << gpio->pin);
}

/* =============================================================================== */
/**
 * @brief Initialiser for a
 *
 * @return @c NULL.
 **
 * =============================================================================== */
void GPIOpin_toggle(GPIOpin_t *gpio) {
  gpio->port->ODR ^= (0b01 << gpio->pin);
}

/* =============================================================================== */
/**
 * @brief Initialiser for a
 *
 * @return @c NULL.
 **
 * =============================================================================== */
void GPIOpin_updateConfig(GPIOpin_t *gpio, GPIO_Config *config) {
  // TODO: implement
}
