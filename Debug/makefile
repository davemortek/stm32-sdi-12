################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32L4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
sdi-12-reader-v1.0.1.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
sdi-12-reader-v1.0.1.list \

OBJCOPY_BIN += \
sdi-12-reader-v1.0.1.bin \


# All Target
all: sdi-12-reader-v1.0.1.elf secondary-outputs

# Tool invocations
sdi-12-reader-v1.0.1.elf: $(OBJS) $(USER_OBJS) C:\Users\david\Documents\Mortek\Firmware\sdi-12-reader-v1.0.1\STM32L412KBUX_FLASH.ld
	arm-none-eabi-gcc -o "sdi-12-reader-v1.0.1.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\david\Documents\Mortek\Firmware\sdi-12-reader-v1.0.1\STM32L412KBUX_FLASH.ld" --specs=nosys.specs -Wl,-Map="sdi-12-reader-v1.0.1.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _scanf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

sdi-12-reader-v1.0.1.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "sdi-12-reader-v1.0.1.list"
	@echo 'Finished building: $@'
	@echo ' '

sdi-12-reader-v1.0.1.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "sdi-12-reader-v1.0.1.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
