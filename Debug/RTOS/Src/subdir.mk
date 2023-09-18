################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../RTOS/Src/osKernel.c 

OBJS += \
./RTOS/Src/osKernel.o 

C_DEPS += \
./RTOS/Src/osKernel.d 


# Each subdirectory must supply rules for building sources it contributes
RTOS/Src/%.o RTOS/Src/%.su RTOS/Src/%.cyclo: ../RTOS/Src/%.c RTOS/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/cesarcruz/STM32CubeIDE/workspace_1.11.2/ISO-I/RTOS/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-RTOS-2f-Src

clean-RTOS-2f-Src:
	-$(RM) ./RTOS/Src/osKernel.cyclo ./RTOS/Src/osKernel.d ./RTOS/Src/osKernel.o ./RTOS/Src/osKernel.su

.PHONY: clean-RTOS-2f-Src

