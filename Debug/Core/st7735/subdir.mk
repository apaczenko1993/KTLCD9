################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/st7735/fonts.c \
../Core/st7735/st7735.c 

OBJS += \
./Core/st7735/fonts.o \
./Core/st7735/st7735.o 

C_DEPS += \
./Core/st7735/fonts.d \
./Core/st7735/st7735.d 


# Each subdirectory must supply rules for building sources it contributes
Core/st7735/%.o Core/st7735/%.su Core/st7735/%.cyclo: ../Core/st7735/%.c Core/st7735/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-st7735

clean-Core-2f-st7735:
	-$(RM) ./Core/st7735/fonts.cyclo ./Core/st7735/fonts.d ./Core/st7735/fonts.o ./Core/st7735/fonts.su ./Core/st7735/st7735.cyclo ./Core/st7735/st7735.d ./Core/st7735/st7735.o ./Core/st7735/st7735.su

.PHONY: clean-Core-2f-st7735

