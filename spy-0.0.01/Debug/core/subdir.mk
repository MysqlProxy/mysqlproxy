################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../core/spy_conf_file.c \
../core/spy_string.c 

OBJS += \
./core/spy_conf_file.o \
./core/spy_string.o 

C_DEPS += \
./core/spy_conf_file.d \
./core/spy_string.d 


# Each subdirectory must supply rules for building sources it contributes
core/%.o: ../core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_SPY_STRING_UNIT_TEST_ -I"/home/terry/workspace/spy-0.0.01/core" -I"/home/terry/workspace/spy-0.0.01/os/unix" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


