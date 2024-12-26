#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("LA");
MODULE_DESCRIPTION("A GPIO interrupt LKM");

/* Variable contains pin number of an interrupt controller to which GPIO 17 is mapped to */
unsigned int irq_number;
static irqreturn_t gpio_irq_handler(int irq, void *dev_id) {
	printk("gpio_irq: Interrupt was triggered and ISR was called!\n");
	return IRQ_HANDLED;
}

/**
 * @brief The interrupt service routine is called, when the interrupt is triggered
 */

/**
 * @brief This function is called when the module is added to the kernel
 */
static int __init ModuleInit(void) {
	printk("GPIO irq: Loading module...\n");
	
	/* Setup GPIO */
	if(gpio_request(17, "rpi-gpio-17")) {
		printk("Error! \n Can't initialize GPIO17 \n");
		return -1;
	}

	/* Setup GPIO 17 direction*/
	if(gpio_direction_input(17)) {
		printk("Error!\nCan not set GPIO 17 to input!\n");
		gpio_free(17);
		return -1;
	}
	/* Setup interrupt */
	irq_number = gpio_to_irq(17);

	if(request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0) {
		printk("Error!\nCan not request interrupt nr.: %d\n", irq_number);
		gpio_free(17);
		return -1;
	}

	printk("Done!\n");
	printk("GPIO 17 is mapped to IRQ Nr.: %d\n", irq_number);
	return 0;
}

/**
 * @brief This function is called when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	printk("gpio_irq: Unloading module... ");
	free_irq(irq_number, NULL);
	gpio_free(17);
	printk("Done\n");

}

module_init(ModuleInit);
module_exit(ModuleExit);