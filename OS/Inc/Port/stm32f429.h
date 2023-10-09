/*
 * stm32f429.h
 *
 *  Created on: Oct 8, 2023
 *      Author: cesarcruz
 */

#ifndef INC_STM32F429_H_
#define INC_STM32F429_H_


#include "stm32f429xx.h"
#include "osKernel.h"

#define IRQ_NUMBER      91                  /* Number of interrupts supported by the MCU */

typedef IRQn_Type   osIRQnType;             /* STM32F4XX interrupt number definition */
typedef void (*IRQHandler)(void* data);     /* Prototype of function */


typedef struct
{
	IRQHandler  handler;    // Function served by the IRQ.
	void*       data;		// Data that is passed to the function that services the IRQ.
}osIRQVector;


extern osIRQVector irqVector[IRQ_NUMBER];


void osIRQHandler(osIRQnType irqType);

#endif // INC_PORTSTM32F429ZI_H_

