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

static void _GPIOpin_init(GPIO_TypeDef *, GPIO_Pin, GPIO_Config);

/* =============================================================================== */
/**
 * @brief Initialiser for a GPIO peripheral pin interface.
 *
 * @param port   Pointer to the GPIO_TypeDef struct representing the pin's port.
 * @param pin    Enum quantified value of the pin's position in its port.
 * @param config Pointer to GPIO_Config struct for initial configuration.
 *               This may be passed as \c NULL to initialise a default
 *               configuration.
 *
 * @return Initialised GPIOpin_t struct.
 **
 * =============================================================================== */
GPIOpin_t GPIOpin_init(GPIO_TypeDef *port, GPIO_Pin pin, GPIO_Config *config) {
  // Early return error struct if port is NULL
  if (port == NULL)
    return (GPIOpin_t){.port = NULL};

  // Create GPIO struct from parameters and initialise methods
  GPIOpin_t GPIO;
  GPIO.port         = port;
  GPIO.pin          = pin;
  GPIO.set          = GPIOpin_set;
  GPIO.reset        = GPIOpin_reset;
  GPIO.toggle       = GPIOpin_toggle;
  GPIO.updateConfig = GPIOpin_updateConfig;

  // Update config and enable peripheral
  GPIOpin_updateConfig(&GPIO, config);

  // Return the new GPIO struct
  return GPIO;
}

// ALLOW FORMATTING
#ifndef DOXYGEN_PRIVATE

/* =============================================================================== */
/**
 * @brief   Private initialiser for GPIO registers.
 * @details Enables and resets the GPIO port in RCC and sets configuration registers.
 *
 * @param port   Pointer to the GPIO_TypeDef struct representing the pin's port.
 * @param pin    Enum quantified value of the pin's position in its port.
 * @param config Pointer to GPIO_Config struct for initial configuration.
 *               This may be passed as \c NULL to initialise a default
 *               configuration. @see GPIO_Config
 *
 * @return \c NULL.
 **
 * =============================================================================== */
static void _GPIOpin_init(GPIO_TypeDef *port, GPIO_Pin pin, GPIO_Config config) {

  // Get index of supplied port by subtracting GPIOA address and dividing by size
  int portIndex = ((uint32_t)port - GPIOA_BASE) / GPIO_PERIPHERAL_SIZE;

  // Enable GPIO peripheral and configure port registers for selected pin
  RCC->AHB1ENR  |= (0b01 << portIndex);                      // Enable port in RCC
  RCC->AHB1RSTR |= (0b01 << portIndex);                      // Reset port
  __ASM("NOP");                                              //
  __ASM("NOP");                                              //
  RCC->AHB1RSTR &= ~(0b01 << portIndex);                     // Clear reset

  port->MODER   &= ~(0b11 << (2 * pin));                     // Clear MODER bits for pin
  port->MODER   |= (config.mode << (2 * pin));               // Shift in mode bits from config

  port->OTYPER  &= ~(0b01 << pin);                           // Clear OTYPE bits for pin
  port->OTYPER  |= (config.type << pin);                     // Shift in type bits from config

  port->OSPEEDR &= ~(0b11 << (2 * pin));                     // Clear OSPEEDR bits for pin
  port->OSPEEDR |= (config.speed << (2 * pin));              // Shift in speed bits from config

  port->PUPDR   &= ~(0b11 << (2 * pin));                     // Clear PUPDR bits for pin
  port->PUPDR   |= (config.pupd << (2 * pin));               // Shift in pupd bits from config

  uint32_t afr   = (pin <= 7) ? port->AFR[0] : port->AFR[1]; // Select AFRL (pin<=7) or AFRH (pin>7)
  afr           &= ~(0b1111 << (4 * pin));                   // Clear AFR bits for pin
  afr           |= (config.afr << (4 * pin));                // Shift in afr bits from config
}

#endif

/* =============================================================================== */
/**
 * @brief   Set the selected GPIO pin
 * @details Uses a logic OR to set the pin's current value in the data register.
 *
 * @param gpio Pointer to GPIOpin_t struct.
 *
 * @return @c NULL.
 **
 * =============================================================================== */
void GPIOpin_set(GPIOpin_t *gpio) {
  gpio->port->ODR |= (0b01 << gpio->pin);
}

/* =============================================================================== */
/**
 * @brief   Clear the selected GPIO pin
 * @details Uses a logic AND to clear the pin's current value in the data register.
 *
 * @param gpio Pointer to GPIOpin_t struct.
 *
 * @return @c NULL.
 **
 * =============================================================================== */
void GPIOpin_reset(GPIOpin_t *gpio) {
  gpio->port->ODR &= ~(0b01 << gpio->pin);
}

/* =============================================================================== */
/**
 * @brief   Toggle the selected GPIO pin
 * @details Uses a logic XOR to invert the pin's current value in the data register.
 *
 * @param gpio Pointer to GPIOpin_t struct.
 *
 * @return @c NULL.
 **
 * =============================================================================== */
void GPIOpin_toggle(GPIOpin_t *gpio) {
  gpio->port->ODR ^= (0b01 << gpio->pin);
}

/* =============================================================================== */
/**
 * @brief   Update GPIO pin configuration
 * @details Uses the provided configuration to update the GPIO registers and resets the
 *          associated port in the RCC.
 *          As with initialisation, passing \c NULL will set the default config.
 *
 * @param gpio Pointer to GPIOpin_t struct.
 *
 * @return @c NULL.
 **
 * =============================================================================== */
void GPIOpin_updateConfig(GPIOpin_t *gpio, GPIO_Config *config) {
  // Initialise config with default values if passed NULL.
  if (config == NULL) {
    config = &(GPIO_Config){
        GPIO_MODE_OUTPUT,   // I/O direction output
        GPIO_TYPE_PUSHPULL, // Push-pull output type
        GPIO_SPEED_HIGH,    // High speed output
        GPIO_PUPD_NONE,     // No pull-up, pull-down
        GPIO_AF0,           // Alternate function 0
    };
  }

  // Update peripheral with new config
  gpio->config = *config;

  // Initialise GPIO registers and enable peripheral
  _GPIOpin_init(gpio->port, gpio->pin, *config);
}
