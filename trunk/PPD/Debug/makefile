################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: PPD

dependents:
	-cd /home/utn_so/Desarrollo/Workspace/C-Talks/Debug && $(MAKE) all
	-cd /home/utn_so/Desarrollo/Workspace/Common-Libs/Debug && $(MAKE) all

# Tool invocations
PPD: $(OBJS) $(USER_OBJS) /home/utn_so/Desarrollo/Workspace/C-Talks/Debug/libC-Talks.so /home/utn_so/Desarrollo/Workspace/Common-Libs/Debug/libCommon-Libs.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utn_so/Desarrollo/Workspace/C-Talks/Debug" -L"/home/utn_so/Desarrollo/Workspace/Common-Libs/Debug" -o "PPD" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) PPD
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utn_so/Desarrollo/Workspace/C-Talks/Debug/libC-Talks.so:
/home/utn_so/Desarrollo/Workspace/Common-Libs/Debug/libCommon-Libs.so:

-include ../makefile.targets
