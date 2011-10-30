################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../PPD.c \
../funcionesViejas.c 

OBJS += \
./PPD.o \
./funcionesViejas.o 

C_DEPS += \
./PPD.d \
./funcionesViejas.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c99 -I"/home/utn_so/Desarrollo/Workspace/Common-Libs" -I"/home/utn_so/Desarrollo/Workspace/C-Talks" -I"/home/utn_so/Desarrollo/Workspace/C-Talks/linux/common-libs/src/lib" -I"/home/utn_so/Desarrollo/Workspace/C-Talks/linux/common-libs/src/lib/collections" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


