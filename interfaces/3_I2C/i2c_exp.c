#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int main(int argc, char *argv[]) {
    int i2c_bus;
    uint8_t data[2];

    /* Let's open the I2c bus */
    i2c_bus = open("/dev/i2c-1", O_RDWR);

    if(i2c_bus < 0) {
        perror("Error opening I2C Bus");
        return -1;
    }

    /* set the slave address */
    if(ioctl(i2c_bus, I2C_SLAVE, 0x20) < 0) { /* 0x20 is the phy address of the i2c slave */
        perror("Error setting slave address");
		close(i2c_bus);
        return -1;
    }

    /* Now access the bus */
    /* Set the GPIOA's direction - all gpios of gpioA will be input but gpio0 is output*/
    data[0] = 0x0;    /* Address of the input ctrl register of slave device */ /*data0-adress*/
    data[1] = 0xfe;  /*data1-value*/

    if(write(i2c_bus,data, sizeof(data)) != 2) {
        perror("Error writing to I2C bus");
        close (i2c_bus);
        return -1;
    }
    /* Set the LED's state */
    data[0] = 0x14;   /* Address of the out ctrl register of slave*/
    if(argc > 1)
        data[1] = atoi(argv[1]) > 0;
    else    
        data[1] = 0x0;

    if(write(i2c_bus,data, sizeof(data)) != 2) {
        perror("Error writing to I2C bus");
        close (i2c_bus);
        return -1;
    }

    /* Let's read the button's state */
    /* Frist write out the address */
    data[0] = 0x12;
        if(write(i2c_bus,data, 1) != 1) {
        perror("Error writing to I2C bus");
        close (i2c_bus);
        return -1;
    }

    /* Let's read the value back */
    if(read(i2c_bus,data, 1) != 1) {
        perror("Error reading the I2C bus");
        close (i2c_bus);
        return -1;
    }

    printf("Button is %s\n", (data[0] & (1<<1)) ? "Pressed" : "Not pressed");
    close(i2c_bus);
    return 0;
}