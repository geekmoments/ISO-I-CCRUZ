#ifndef INC_GPIOWRAPPER_H_
#define INC_GPIOWRAPPER_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f429xx.h"

void gpioSetLevel(uint16_t pin, uint32_t port, bool value);

bool gpioGetLevel(uint16_t pin, uint32_t port);

#endif /* INC_GPIOWRAPPER_H_ */
