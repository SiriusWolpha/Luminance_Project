################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utilities/fsl_assert.c \
../utilities/fsl_debug_console.c \
../utilities/fsl_str.c 

C_DEPS += \
./utilities/fsl_assert.d \
./utilities/fsl_debug_console.d \
./utilities/fsl_str.d 

OBJS += \
./utilities/fsl_assert.o \
./utilities/fsl_debug_console.o \
./utilities/fsl_str.o 


# Each subdirectory must supply rules for building sources it contributes
utilities/%.o: ../utilities/%.c utilities/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MIMXRT1021DAG5A -DCPU_MIMXRT1021DAG5A_cm7 -DSDK_OS_BAREMETAL -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSDK_DEBUGCONSOLE_UART -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"D:\Data\MCUXpresso_workspace\Luminance_Project\board" -I"D:\Data\MCUXpresso_workspace\Luminance_Project\device" -I"D:\Data\MCUXpresso_workspace\Luminance_Project\component\serial_manager" -I"D:\Data\MCUXpresso_workspace\Luminance_Project\component\uart" -I"D:\Data\MCUXpresso_workspace\Luminance_Project\drivers" -I"D:\Data\MCUXpresso_workspace\Luminance_Project\CMSIS" -I"D:\Data\MCUXpresso_workspace\Luminance_Project\xip" -I"D:\Data\MCUXpresso_workspace\Luminance_Project\utilities" -I"D:\Data\MCUXpresso_workspace\Luminance_Project\component\lists" -I"D:\Data\MCUXpresso_workspace\Luminance_Project\CMSIS_driver\Include" -I"D:\Data\MCUXpresso_workspace\Luminance_Project\freertos\freertos-kernel\include" -I"D:\Data\MCUXpresso_workspace\Luminance_Project\freertos\freertos-kernel\portable\GCC\ARM_CM4F" -I"D:\Data\MCUXpresso_workspace\Luminance_Project\source" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-utilities

clean-utilities:
	-$(RM) ./utilities/fsl_assert.d ./utilities/fsl_assert.o ./utilities/fsl_debug_console.d ./utilities/fsl_debug_console.o ./utilities/fsl_str.d ./utilities/fsl_str.o

.PHONY: clean-utilities

