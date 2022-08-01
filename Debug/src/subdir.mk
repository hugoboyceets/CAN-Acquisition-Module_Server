################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CAN-Acquisition-Module_Server.c 

C_DEPS += \
./src/CAN-Acquisition-Module_Server.d 

OBJS += \
./src/CAN-Acquisition-Module_Server.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/CAN-Acquisition-Module_Server.d ./src/CAN-Acquisition-Module_Server.o

.PHONY: clean-src

