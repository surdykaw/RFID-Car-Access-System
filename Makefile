# MCU configuration
MCU=atmega8a
MCU_SHORT=m8a
F_CPU=4000000UL

# Toolchain
CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size

# Compiler flags
CFLAGS=-mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Ifirmware/include

# Project settings
TARGET=firmware
BUILD_DIR=build

# Source files
SRCS=firmware/main.c \
     firmware/src/spi-config.c \
     firmware/src/mfrc522.c \
     firmware/src/wdt-config.c

ELF=$(BUILD_DIR)/$(TARGET).elf
HEX=$(BUILD_DIR)/$(TARGET).hex

# Build binaries
build:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(ELF) $(SRCS)
	$(OBJCOPY) -O ihex $(ELF) $(HEX)
	$(SIZE) $(ELF)

# Flash firmware to MCU
flashupdate:
	avrdude -c usbasp -p $(MCU_SHORT) -U flash:w:$(HEX)

# Remove build files
clean:
	rm -rf $(BUILD_DIR)