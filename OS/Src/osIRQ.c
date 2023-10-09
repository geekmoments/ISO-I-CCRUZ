/*
 * osIRQ.c
 *
 *  Created on: Oct 8, 2023
 *      Author: cesarcruz
 */

#include "osIRQ.h"
#include "osKernel.h"//**


bool osRegisterIRQ(osIRQnType irqType, IRQHandler function, void *data)
{
    if (irqType >= IRQ_NUMBER || irqType < 0 || function == NULL || irqVector[irqType].handler != NULL) {
        return false;
    }

    osIRQVector vector;
    vector.handler = function;
    vector.data = data;

    irqVector[irqType] = vector;

    NVIC_ClearPendingIRQ(irqType);
    NVIC_EnableIRQ(irqType);

    return true;
}

bool osUnregisterIRQ(osIRQnType irqType)
{
	if (irqType > IRQ_NUMBER || irqType < 0)
		return false;

	osIRQVector vector;
	vector.handler = NULL;
	vector.data = NULL;

	irqVector[irqType] = vector;

	NVIC_ClearPendingIRQ(irqType);
	NVIC_DisableIRQ(irqType);
    return true;
}

