#include "GPIOWrapper.h"
#include "stm32f4xx_hal.h"

void gpioSetLevel(uint16_t pin, uint32_t port, bool value)
{
    // Realiza la conversión de uint32_t a GPIO_TypeDef*.
    GPIO_TypeDef* portPointer = (GPIO_TypeDef*)port;

    // Llama a la función HAL_GPIO_WritePin con el puntero convertido.
    GPIO_PinState level = (value) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(portPointer, pin, level);
}

bool gpioGetLevel(uint16_t pin, uint32_t port)
{
    // Realiza la conversión de uint32_t a GPIO_TypeDef*.
    GPIO_TypeDef* portPointer = (GPIO_TypeDef*)port;

    // Llama a la función HAL_GPIO_ReadPin con el puntero convertido.
    GPIO_PinState buttonState = HAL_GPIO_ReadPin(portPointer, pin);
    if (buttonState == GPIO_PIN_SET) {
        return true;
    } else {
        return false;
    }
}
