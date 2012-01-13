################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../os/unix/spy_errno.c \
../os/unix/spy_process.c \
../os/unix/spy_time.c 

OBJS += \
./os/unix/spy_errno.o \
./os/unix/spy_process.o \
./os/unix/spy_time.o 

C_DEPS += \
./os/unix/spy_errno.d \
./os/unix/spy_process.d \
./os/unix/spy_time.d 


# Each subdirectory must supply rules for building sources it contributes
os/unix/%.o: ../os/unix/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DSPY_SMP -D_SPY_LUA_UNIT_TEST_ -U_SPY_MATH_UNIT_TEST_ -U_SPY_MINHEAP_UNIT_TEST_ -U_SPY_UNIT_TEST_ -U_SPY_TIMES_UNIT_TEST -U_SPY_STRING_UNIT_TEST_ -I"/home/terry/github/spy-0.0.01/core" -I"/home/terry/github/spy-0.0.01/proxy" -I"/home/terry/github/spy-0.0.01/mysql" -I"/home/terry/github/spy-0.0.01/event" -I/usr/include/lua5.1 -I"/home/terry/github/spy-0.0.01/os/unix" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


