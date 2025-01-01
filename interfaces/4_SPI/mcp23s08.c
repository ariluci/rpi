#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

uint8_t mode = 0;
uint32_t speed = 500000; /* 500KHz */
uint8_t bits = 8;  /* Number of bits to write at every SPI access */


/*
 * @brief Function makes a SPI transfer
 * i2c had read and write transaction. spi works different: with each byte you can write out, read in a byte
 *
 * @param fd        File derscriptor to SPI bus
 * @param data      Data array with output (write) data
 *                  will be overwritten with the received input data
 * @param length    Length of data array  
 
 */
void spi_transfer(int fd, uint8_t *data, int length) {
    struct spi_ioc_transfer spi[length];
    int i;

    /* Setup the transfer struct */
    for(i=0; i < length; i++) {
        memset(&spi[i], 0, sizeof(struct spi_ioc_transfer));
        spi[i].tx_buf = (unsigned long) (data+i); /* Pointer to curent element of data array */
        spi[i].rx_buf = (unsigned long) (data+i); /* Where we want to write our data 
                                                     we could use different data array but we will just overwrite the same array */
        spi[i].len = 1; /* in bytes */
        spi[i].speed_hz = speed;        
        spi[i].bits_per_word = bits;           
    }

    /* Do the transfer */
    if(ioctl(fd, SPI_IOC_MESSAGE(length), spi) < 0) {
        perror("Error transfering data over SPI bus");
        close(fd);
        exit(-1);
    }
}

int main(int argc, char *argv[])  {
    int fd; 
    uint8_t data[3]; /* Array to store input and output data */
    
    /* Open SPI bus file descriptor */
    fd = open("/dev/spidev0.0", O_RDWR);
    if (fd < 0) {
        perror("Error opening SPI Bus");
        return -1;
    }
    /* Setup of the SPI Bus */
    if(ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
        perror("Error setting the SPI mode");
	    close(fd);
        return -1;
    }
    if(ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        perror("Error setting the SPI speed");
	    close(fd); 
        return -1;
    }
    if(ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
        perror("Error setting SPI bits per word");
		close(fd);
        return -1;
    }

    /* Ready for SPI accesses */

    /* Setup GPIO's direction */
    data[0] = 0x40;  /* From mcp32s08 datasheet IODIRs first 5 bits are fixed to 01000 for SPI addresig,
                        next 2 are adressing bits set to 00, lsb is 0 for write*/
    data[1] = 0x0;   /* From mcp32s08 datasheet IODIRs Address we want to write to is 0x0 */
    data[2] = 0xfe;   /*this means all gpio are input, only gpio1 is an output */
    spi_transfer(fd, data, 3);


    /* Set LED */
    data[0] = 0x40; /* This is still a write */
    data[1] = 0xa;  /* Adress is 0xa for Out Latch */ 
    if(argc > 1)
        data[2] = atoi(argv[1]) > 0;
    else    
          data[2] = 0x0;
    spi_transfer(fd, data, 3);

    /* Read button */
    data[0] = 0x41; /* This is a read */
    data[1] = 0x9;  /*mcp32s08 adrress of GPIO*/
    data[2] = 0x00;  /* DK */
    spi_transfer(fd, data, 3);

    printf("Button is %s\n", ((data[2] & (1<<1)) > 0) ? "pressed" : "not pressed");
    close(fd);
    return 0;
}
