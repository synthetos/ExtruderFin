###############################################################################
# Makefile for the project extruderfin
###############################################################################

## General Flags
PROJECT = extruderfin
MCU = atmega328p
TARGET = extruderfin.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99       -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=extruderfin.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Libraries
LIBS = -lm 

## Objects that must be built in order to link
OBJECTS = util.o config.o config_app.o heater.o json_parser.o main.o report.o xio_usart.o xio_file.o xio_spi.o controller.o hardware.o xio.o text_parser.o persistence.o sensor_thermo.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) extruderfin.hex extruderfin.eep extruderfin.lss size

## Compile
util.o: ../util.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

config.o: ../config.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

config_app.o: ../config_app.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

heater.o: ../heater.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

json_parser.o: ../json_parser.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

report.o: ../report.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

xio_usart.o: ../xio/xio_usart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

xio_file.o: ../xio/xio_file.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

xio_spi.o: ../xio/xio_spi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

controller.o: ../controller.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hardware.o: ../hardware.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

xio.o: ../xio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

text_parser.o: ../text_parser.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

persistence.o: ../persistence.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sensor_thermo.o: ../sensor_thermo.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) extruderfin.elf dep/* extruderfin.hex extruderfin.eep extruderfin.lss extruderfin.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

