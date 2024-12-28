#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/poll.h>
#include <linux/wait.h>
#define MYMAJOR 64

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("LA");
MODULE_DESCRIPTION("Kernel Module which uses poll to notify an userspace app when");

/* Variable contains pin number of interrupt controller to which GPIO 17 is mapped to */
unsigned int irq_number;
static int irq_ready = 0;
static wait_queue_head_t waitqueue;

/**
 * @brief Interrupt service routine is called, when interrupt is triggered 
 */
static irqreturn_t gpio_irq_poll_handler(int irq, void *dev_id) {
    printk("gpio_irq_poll: Interrupt was triggered and ISR was called!\n");
    irq_ready = 1;
    wake_up(&waitqueue);
    return  IRQ_HANDLED;
}

/**
 * @brief Poll callback to allow userspace app to poll for button being pressed
 */
 static unsigned int my_poll(struct file *file, poll_table *wait) {
    poll_wait(file, &waitqueue, wait);
    if(irq_ready == 1) {
        irq_ready = 0;
        return POLLIN;
    }
    return 0;
 }


static struct file_operations fops = {
    .owner = THIS_MODULE,
    .poll = my_poll
};

/**
 * @brief This function is called, when the module is added to the kernel
 */
static int __init ModuleInit(void) {
    printk("gpio_irq: Loading module...\n");

    /* Init waitqueue */
    init_waitqueue_head(&waitqueue);

    /* Setup GPIO */
    if(gpio_request(17, "rpi-gpio-17")) {
        printk("Error!\nCould not allocate GPIO 17\n");
        return -1;
    }
    /* Setup GPIO 17 direction */
    if(gpio_direction_input(17)) {
        printk("Error!\nCan not set GPIO 17 to input!\n");
        gpio_free(17);
        return -1;
    }

    gpio_set_debounce(17, 300);

    /* Setup the interrupt */
    irq_number = gpio_to_irq(17);

    if(request_irq(irq_number, gpio_irq_poll_handler, IRQF_TRIGGER_RISING, "my_gpio_irq_poll", NULL) != 0) {
        printk("Error!\nCould not request interrupt number: %d!\n", irq_number);
        gpio_free(17);
        return -1;
    }

    if(register_chrdev(MYMAJOR, "gpio_irq_poll", &fops) < 0) {
        printk("Error!\nCould not register device number!\n");
        gpio_free(17);
        free_irq(irq_number, NULL);
    }

    printk("Done!\n");
    printk("GPIO 17 is mapped to IRQ Nr.: %d", irq_number);
    return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
    printk("gpio_irq_poll: Unloading module...");
    free_irq(irq_number, NULL);
    gpio_free(17);
    unregister_chrdev(MYMAJOR, "gpio_irq_poll");
    printk("Done!\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
