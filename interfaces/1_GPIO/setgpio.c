#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <linux/gpio.h>

int main() {
    int fd;
    struct gpiohandle_request led, button;
    struct gpiohandle_data data;

    /* Open the gpio device file */
    fd = open("/dev/gpiochip0", O_RDWR);
    if (fd < 0) {
        perror("Could not open file gpiochip0\n");
        return -1;
    }

    /* Setup LED 16 to output */
    led.flags = GPIOHANDLE_REQUEST_OUTPUT;
    strcpy(led.consumer_label, "LED");   
    memset(led.default_values, 0, sizeof(led.default_values));
    led.lines = 1;
    led.lineoffsets[0] = 16; /* Pin number */

    if(ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &led) < 0) {
        perror("Error setting GPIO 16 to output");
        close(fd);
        return -1;
    }

        /* Setup LED 2 to input  */
    button.flags = GPIOHANDLE_REQUEST_INPUT;
    strcpy(button.consumer_label, "BUTTON");   
    memset(button.default_values, 0, sizeof(button.default_values));
    button.lines = 1;
    button.lineoffsets[0] = 20; /* Pin number */

    if(ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &button) < 0) {
        perror("Error setting GPIO 20 to input");
        close(led.fd);
        close(fd);
        return -1;
    }

    /* Let's set the LED */
    data.values[0] = 1;
    if(ioctl(led.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0)
        perror("Error setting the GPIO to 1");

    /* Read button state */
    if(ioctl(button.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0)
         perror("Error reading the GPIO");
    printf("Button is %s\n", (data.values[0] > 0) ? "pressed" : "not pressed");

    sleep(5); /* Wait 5 seconds before turning of */

    data.values[0] = 0;
    if(ioctl(led.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0)
        perror("Error setting the GPIO to 0");

    close(fd);
    close(led.fd);
    close(button.fd);
    return 0;
}


