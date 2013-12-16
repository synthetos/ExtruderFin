################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../xio


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../config.c \
../config_app.c \
../controller.c \
../heater.c \
../json_parser.c \
../main.c \
../persistence.c \
../pid.c \
../report.c \
../hardware.c \
../sensor_thermo.c \
../test.c \
../text_parser.c \
../util.c \
../xio.c \
../xio/xio_file.c \
../xio/xio_spi.c \
../xio/xio_usart.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
config.o \
config_app.o \
controller.o \
heater.o \
json_parser.o \
main.o \
persistence.o \
pid.o \
report.o \
hardware.o \
sensor_thermo.o \
test.o \
text_parser.o \
util.o \
xio.o \
xio/xio_file.o \
xio/xio_spi.o \
xio/xio_usart.o

OBJS_AS_ARGS +=  \
config.o \
config_app.o \
controller.o \
heater.o \
json_parser.o \
main.o \
persistence.o \
pid.o \
report.o \
hardware.o \
sensor_thermo.o \
test.o \
text_parser.o \
util.o \
xio.o \
xio/xio_file.o \
xio/xio_spi.o \
xio/xio_usart.o

C_DEPS +=  \
config.d \
config_app.d \
controller.d \
heater.d \
json_parser.d \
main.d \
persistence.d \
pid.d \
report.d \
hardware.d \
sensor_thermo.d \
test.d \
text_parser.d \
util.d \
xio.d \
xio/xio_file.d \
xio/xio_spi.d \
xio/xio_usart.d

C_DEPS_AS_ARGS +=  \
config.d \
config_app.d \
controller.d \
heater.d \
json_parser.d \
main.d \
persistence.d \
pid.d \
report.d \
hardware.d \
sensor_thermo.d \
test.d \
text_parser.d \
util.d \
xio.d \
xio/xio_file.d \
xio/xio_spi.d \
xio/xio_usart.d

OUTPUT_FILE_PATH +=extruderfin.elf

OUTPUT_FILE_PATH_AS_ARGS +=extruderfin.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

# AVR32/GNU C Compiler





































./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 3.4.2
	$(QUOTE)C:\Program Files\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -funsigned-char -funsigned-bitfields -DDEBUG -DF_CPU=32000000UL  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega328p -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<"
	@echo Finished building: $<
	

xio/%.o: ../xio/%.c
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 3.4.2
	$(QUOTE)C:\Program Files\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -funsigned-char -funsigned-bitfields -DDEBUG -DF_CPU=32000000UL  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega328p -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<"
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP)
	@echo Building target: $@
	@echo Invoking: AVR8/GNU Linker : 3.4.2
	$(QUOTE)C:\Program Files\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="extruderfin.map" -Wl,-u,vfprintf -Wl,--start-group -Wl,-lm -Wl,-lprintf_flt  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega328p 
	@echo Finished building target: $@
	"C:\Program Files\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "extruderfin.elf" "extruderfin.hex"
	"C:\Program Files\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "extruderfin.elf" "extruderfin.eep" || exit 0
	"C:\Program Files\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "extruderfin.elf" > "extruderfin.lss"
	"C:\Program Files\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature  "extruderfin.elf" "extruderfin.srec"
	"C:\Program Files\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.2.1002\avr8-gnu-toolchain\bin\avr-size.exe" "extruderfin.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "extruderfin.elf" "extruderfin.a" "extruderfin.hex" "extruderfin.lss" "extruderfin.eep" "extruderfin.map" "extruderfin.srec"
	