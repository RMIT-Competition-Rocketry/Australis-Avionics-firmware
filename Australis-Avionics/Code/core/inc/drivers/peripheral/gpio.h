#ifndef _GPIO_H
#define _GPIO_H

#include "gpiopin.h"

#define GPIO_MODER_FIELD_SIZE 	0x02
#define GPIO_MODER(pin)					pin * GPIO_MODER_FIELD_SIZE
#define GPIO_PULL_UP     				0x01
#define GPIO_PUPDR_FIELD_SIZE 	0x02
#define GPIO_PUPDR(pin)					pin * GPIO_PUPDR_FIELD_SIZE
#define GPIO_OSPEEDR_FIELD_SIZE 0x02
#define GPIO_OSPEEDR(pin)				pin * GPIO_OSPEEDR_FIELD_SIZE

#endif
