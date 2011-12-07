################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../core/spy.c \
../core/spy_alloc.c \
../core/spy_conf_file.c \
../core/spy_file.c \
../core/spy_largealloc.c \
../core/spy_log.c \
../core/spy_lua.c \
../core/spy_math.c \
../core/spy_string.c \
../core/spy_times.c 

OBJS += \
./core/spy.o \
./core/spy_alloc.o \
./core/spy_conf_file.o \
./core/spy_file.o \
./core/spy_largealloc.o \
./core/spy_log.o \
./core/spy_lua.o \
./core/spy_math.o \
./core/spy_string.o \
./core/spy_times.o 

C_DEPS += \
./core/spy.d \
./core/spy_alloc.d \
./core/spy_conf_file.d \
./core/spy_file.d \
./core/spy_largealloc.d \
./core/spy_log.d \
./core/spy_lua.d \
./core/spy_math.d \
./core/spy_string.d \
./core/spy_times.d 


# Each subdirectory must supply rules for building sources it contributes
core/%.o: ../core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_SPY_LUA_UNIT_TEST_ -U_SPY_MATH_UNIT_TEST_ -U_SPY_TIMES_UNIT_TEST -U_SPY_STRING_UNIT_TEST_ -I"/home/terry/github/spy-0.0.01/core" -I/usr/include/lua5.1 -I"/home/terry/github/spy-0.0.01/os/unix" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


