#include <string.h>

#include "SerialWrapper.h"
#include "usart.h"

void serialPrint(char* buffer)
{
	 HAL_UART_Transmit(&huart3, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

}
