DEVICE = atmega328p
MCU = atmega328p
AVRDUDE_DEVICE = m328p

CC = avr-gcc
CFLAGS = -std=c99 -g -Wall -mcall-prologues -mmcu=$(MCU) -Os
LDFLAGS = -Wl,-gc-sections -lpololu_$(DEVICE) -Wl,-relax

OBJ2HEX = avr-objcopy

AVRDUDE = avrdude
PORT = /dev/tty.usbmodem00088291

HEXFILE = image.hex
OBJECT_FILES = $(patsubst %.c,%.o,$(SRC))

all: $(HEXFILE)

clean::
	rm -f *.o *.hex *.obj

program: $(HEXFILE)
	$(AVRDUDE) -p $(AVRDUDE_DEVICE) -c avrisp2 -P $(PORT) -U flash:w:$(HEXFILE)

%.hex: %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

%.obj: $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(OBJECT_FILES) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o    $@ $<

.PHONY: all clean program
.PRECIOUS: %.o
.DEFAULT_GOAL := all
