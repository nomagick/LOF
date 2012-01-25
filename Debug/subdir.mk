################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LOF_connection.c \
../LOF_debug.c \
../LOF_list.c \
../LOF_message.c \
../LOF_sip.c 

OBJS += \
./LOF_connection.o \
./LOF_debug.o \
./LOF_list.o \
./LOF_message.o \
./LOF_sip.o 

C_DEPS += \
./LOF_connection.d \
./LOF_debug.d \
./LOF_list.d \
./LOF_message.d \
./LOF_sip.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


