$(shell mkdir -p build)
CC = avr-gcc
MCU = atmega328p
F_CPU = 16000000UL
BAUD = 115200
BASE_CHIP2 = /dev/ttyUSB0
BASE_CHIP = /dev/ttyACM0
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))
.PHONY: flash unoflash clean

build: build/main.hex

build/main.hex: build/main.elf
	avr-objcopy -O ihex $< $@

build/main.elf: $(OBJS)
	$(CC) -mmcu=$(MCU) -o $@ $(OBJS)

build/%.o: src/%.c | build
	$(CC) -I include -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -c $< -o $@

build:
	mkdir -p build

flash:
	avrdude -c arduino -b $(BAUD) -p $(MCU) -P $(BASE_CHIP) -U flash:w:build/main.hex:i

unoflash:
	avrdude -c arduino -b $(BAUD) -p $(MCU) -P $(UNO) -U flash:w:build/main.hex:i


clean:
	rm -rf build/

