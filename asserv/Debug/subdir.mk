################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../AFMotor.cpp \
../PID_Beta6.cpp \
../command.cpp \
../control.cpp \
../encoder.cpp \
../fifo.cpp \
../message.cpp \
../pwm.cpp \
../robotstate.cpp 

OBJS += \
./AFMotor.o \
./PID_Beta6.o \
./command.o \
./control.o \
./encoder.o \
./fifo.o \
./message.o \
./pwm.o \
./robotstate.o 

CPP_DEPS += \
./AFMotor.d \
./PID_Beta6.d \
./command.d \
./control.d \
./encoder.d \
./fifo.d \
./message.d \
./pwm.d \
./robotstate.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


