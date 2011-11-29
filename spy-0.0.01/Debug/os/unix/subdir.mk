################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../os/unix/spy_errno.c 

OBJS += \
./os/unix/spy_errno.o 

C_DEPS += \
./os/unix/spy_errno.d 


# Each subdirectory must supply rules for building sources it contributes
os/unix/%.o: ../os/unix/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_SPY_STRING_UNIT_TEST_ -U_SPY_MATH_UNIT_TEST_ -I"/home/terry/github/spy-0.0.01/core" -I"/home/terry/github/spy-0.0.01/os/unix" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


