################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../event/spy_event.c \
../event/spy_event_timer.c 

OBJS += \
./event/spy_event.o \
./event/spy_event_timer.o 

C_DEPS += \
./event/spy_event.d \
./event/spy_event_timer.d 


# Each subdirectory must supply rules for building sources it contributes
event/%.o: ../event/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DSPY_SMP -D_SPY_LUA_UNIT_TEST_ -U_SPY_MATH_UNIT_TEST_ -U_SPY_MINHEAP_UNIT_TEST_ -U_SPY_UNIT_TEST_ -U_SPY_TIMES_UNIT_TEST -U_SPY_STRING_UNIT_TEST_ -I"/home/terry/github/spy-0.0.01/core" -I"/home/terry/github/spy-0.0.01/proxy" -I"/home/terry/github/spy-0.0.01/mysql" -I"/home/terry/github/spy-0.0.01/event" -I/usr/include/lua5.1 -I"/home/terry/github/spy-0.0.01/os/unix" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


