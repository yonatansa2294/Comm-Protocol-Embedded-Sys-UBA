################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/STM32F4xx_BlackPill/Src/stm32f4xx_BlackPill.c 

OBJS += \
./Drivers/BSP/STM32F4xx_BlackPill/Src/stm32f4xx_BlackPill.o 

C_DEPS += \
./Drivers/BSP/STM32F4xx_BlackPill/Src/stm32f4xx_BlackPill.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32F4xx_BlackPill/Src/%.o Drivers/BSP/STM32F4xx_BlackPill/Src/%.su: ../Drivers/BSP/STM32F4xx_BlackPill/Src/%.c Drivers/BSP/STM32F4xx_BlackPill/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I"D:/STM/EX1_SEND_SMS/Inc" -I"D:/STM/EX1_SEND_SMS/Drivers/Core/Inc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I"D:/STM/EX1_SEND_SMS/Drivers/BSP/STM32F4xx_BlackPill/Inc" -I"D:/STM/EX1_SEND_SMS/Drivers/API/Inc" -I"D:/STM/EX1_SEND_SMS/Drivers/SIM800x/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-STM32F4xx_BlackPill-2f-Src

clean-Drivers-2f-BSP-2f-STM32F4xx_BlackPill-2f-Src:
	-$(RM) ./Drivers/BSP/STM32F4xx_BlackPill/Src/stm32f4xx_BlackPill.d ./Drivers/BSP/STM32F4xx_BlackPill/Src/stm32f4xx_BlackPill.o ./Drivers/BSP/STM32F4xx_BlackPill/Src/stm32f4xx_BlackPill.su

.PHONY: clean-Drivers-2f-BSP-2f-STM32F4xx_BlackPill-2f-Src

