#include <iostream>
#include <wiringPiSPI.h>

#define SPI_CHANNEL 0
#define SPI_CLOCK_SPEED 1000000 //1MHz

int main(int argc, char *argv[])
{
    int fd = wiringPiSPISetupMode(SPI_CHANNEL, SPI_CLOCK_SPEED, 0); /* 0 - SPI mode 0, default mode for arduino */
    uint8_t value = 52;

    if (fd == -1) {
        std::cout << "Failed to init SPI communication.\n";
        return -1;
    }
    std::cout << "SPI communication successfully setup.\n";

    /* Transfer data over SPI with Arduino */
    unsigned char buf[2] = {value, 0};  /* With SPI we don’t just send values and then receive other values. 
                                         * With SPI each single communication is a transfer. 
                                         * You send a byte, and while you do that, you receive another one.
                                         */ 
    wiringPiSPIDataRW(SPI_CHANNEL, buf, 2);    
    std::cout << "Value sent from rpi: " << (int)value << "\n";
    std::cout << "Data returned from arduino (after +10 in sketch):   " << +buf[1] << "\n";
 
    return 0;
}


