MCU     = atmega88pa
FREQ    = 8000000UL
SOURCES = main.c
TARGET  = spi_adc

all:
	avr-gcc -Wall -mmcu=$(MCU) -Is -Iinc/ -DF_CPU=$(FREQ) -o $(TARGET).elf
	ovr-objcopy -O ihex $(TARGET).elf $(TARGET).hex

flash:
	sudo avrdude -c usbasp -p m168p -U flash:w$(TARGET).hex

clean:
	rm -rf $(TARGET).elf $(TARGET).hex