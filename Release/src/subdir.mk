################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CAN_messages.c \
../src/main.c \
../src/user_io.c 

C_DEPS += \
./src/CAN_messages.d \
./src/main.d \
./src/user_io.d 

OBJS += \
./src/CAN_messages.o \
./src/main.o \
./src/user_io.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/CAN_messages.d ./src/CAN_messages.o ./src/main.d ./src/main.o ./src/user_io.d ./src/user_io.o

.PHONY: clean-src

