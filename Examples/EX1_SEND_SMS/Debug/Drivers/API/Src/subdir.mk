################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/API/Src/API_Debounce.c \
../Drivers/API/Src/API_DelayNB.c 

OBJS += \
./Drivers/API/Src/API_Debounce.o \
./Drivers/API/Src/API_DelayNB.o 

C_DEPS += \
./Drivers/API/Src/API_Debounce.d \
./Drivers/API/Src/API_DelayNB.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/API/Src/%.o Drivers/API/Src/%.su: ../Drivers/API/Src/%.c Drivers/API/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I"D:/STM/EX1_SEND_SMS/Inc" -I"D:/STM/EX1_SEND_SMS/Drivers/Core/Inc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I"D:/STM/EX1_SEND_SMS/Drivers/BSP/STM32F4xx_BlackPill/Inc" -I"D:/STM/EX1_SEND_SMS/Drivers/API/Inc" -I"D:/STM/EX1_SEND_SMS/Drivers/SIM800x/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-API-2f-Src

clean-Drivers-2f-API-2f-Src:
	-$(RM) ./Drivers/API/Src/API_Debounce.d ./Drivers/API/Src/API_Debounce.o ./Drivers/API/Src/API_Debounce.su ./Drivers/API/Src/API_DelayNB.d ./Drivers/API/Src/API_DelayNB.o ./Drivers/API/Src/API_DelayNB.su

.PHONY: clean-Drivers-2f-API-2f-Src

