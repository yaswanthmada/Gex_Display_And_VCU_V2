################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ADC.c \
../Core/Src/CAN.c \
../Core/Src/DECODE_BATTERY_MANAGEMENT_SYSTEM.c \
../Core/Src/DECODE_MOTOR_CONTROLLER.c \
../Core/Src/DWIN_DISPLAY.c \
../Core/Src/GPIO.c \
../Core/Src/MCP2515.c \
../Core/Src/PRINT_DECODE_BATTERY_MANAGEMENT_SYSTEM.c \
../Core/Src/PROCESS.c \
../Core/Src/RCC.c \
../Core/Src/SPI.c \
../Core/Src/SYSTICK.c \
../Core/Src/TASK_TIMER.c \
../Core/Src/UART.c \
../Core/Src/main.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c 

OBJS += \
./Core/Src/ADC.o \
./Core/Src/CAN.o \
./Core/Src/DECODE_BATTERY_MANAGEMENT_SYSTEM.o \
./Core/Src/DECODE_MOTOR_CONTROLLER.o \
./Core/Src/DWIN_DISPLAY.o \
./Core/Src/GPIO.o \
./Core/Src/MCP2515.o \
./Core/Src/PRINT_DECODE_BATTERY_MANAGEMENT_SYSTEM.o \
./Core/Src/PROCESS.o \
./Core/Src/RCC.o \
./Core/Src/SPI.o \
./Core/Src/SYSTICK.o \
./Core/Src/TASK_TIMER.o \
./Core/Src/UART.o \
./Core/Src/main.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o 

C_DEPS += \
./Core/Src/ADC.d \
./Core/Src/CAN.d \
./Core/Src/DECODE_BATTERY_MANAGEMENT_SYSTEM.d \
./Core/Src/DECODE_MOTOR_CONTROLLER.d \
./Core/Src/DWIN_DISPLAY.d \
./Core/Src/GPIO.d \
./Core/Src/MCP2515.d \
./Core/Src/PRINT_DECODE_BATTERY_MANAGEMENT_SYSTEM.d \
./Core/Src/PROCESS.d \
./Core/Src/RCC.d \
./Core/Src/SPI.d \
./Core/Src/SYSTICK.d \
./Core/Src/TASK_TIMER.d \
./Core/Src/UART.d \
./Core/Src/main.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/ADC.cyclo ./Core/Src/ADC.d ./Core/Src/ADC.o ./Core/Src/ADC.su ./Core/Src/CAN.cyclo ./Core/Src/CAN.d ./Core/Src/CAN.o ./Core/Src/CAN.su ./Core/Src/DECODE_BATTERY_MANAGEMENT_SYSTEM.cyclo ./Core/Src/DECODE_BATTERY_MANAGEMENT_SYSTEM.d ./Core/Src/DECODE_BATTERY_MANAGEMENT_SYSTEM.o ./Core/Src/DECODE_BATTERY_MANAGEMENT_SYSTEM.su ./Core/Src/DECODE_MOTOR_CONTROLLER.cyclo ./Core/Src/DECODE_MOTOR_CONTROLLER.d ./Core/Src/DECODE_MOTOR_CONTROLLER.o ./Core/Src/DECODE_MOTOR_CONTROLLER.su ./Core/Src/DWIN_DISPLAY.cyclo ./Core/Src/DWIN_DISPLAY.d ./Core/Src/DWIN_DISPLAY.o ./Core/Src/DWIN_DISPLAY.su ./Core/Src/GPIO.cyclo ./Core/Src/GPIO.d ./Core/Src/GPIO.o ./Core/Src/GPIO.su ./Core/Src/MCP2515.cyclo ./Core/Src/MCP2515.d ./Core/Src/MCP2515.o ./Core/Src/MCP2515.su ./Core/Src/PRINT_DECODE_BATTERY_MANAGEMENT_SYSTEM.cyclo ./Core/Src/PRINT_DECODE_BATTERY_MANAGEMENT_SYSTEM.d ./Core/Src/PRINT_DECODE_BATTERY_MANAGEMENT_SYSTEM.o ./Core/Src/PRINT_DECODE_BATTERY_MANAGEMENT_SYSTEM.su ./Core/Src/PROCESS.cyclo ./Core/Src/PROCESS.d ./Core/Src/PROCESS.o ./Core/Src/PROCESS.su ./Core/Src/RCC.cyclo ./Core/Src/RCC.d ./Core/Src/RCC.o ./Core/Src/RCC.su ./Core/Src/SPI.cyclo ./Core/Src/SPI.d ./Core/Src/SPI.o ./Core/Src/SPI.su ./Core/Src/SYSTICK.cyclo ./Core/Src/SYSTICK.d ./Core/Src/SYSTICK.o ./Core/Src/SYSTICK.su ./Core/Src/TASK_TIMER.cyclo ./Core/Src/TASK_TIMER.d ./Core/Src/TASK_TIMER.o ./Core/Src/TASK_TIMER.su ./Core/Src/UART.cyclo ./Core/Src/UART.d ./Core/Src/UART.o ./Core/Src/UART.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su

.PHONY: clean-Core-2f-Src

