#include <linux/module.h>
#include <linux/init.h>
#include <linux/completion.h>
#include <linux/jiffies.h>
#include <linux/timer.h>

/* Meta Info */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("LA");
MODULE_DESCRIPTION("A simple test for LKM completions");

static struct timer_list my_timer; /* variable for timer */
static struct completion comp;	   /* variable for completion */

void timer_callback(struct timer_list * data) {
	/* For this example we'll think about this callback 
	 * as an interrupt service routine which is called when
	 * event is triggered, e.g. serial data is received
	 */
	printk("mycompletion - timer expired\n");	
	complete(&comp); /* This will indicate the timer expired 
					  * and after completion was set
					  * code can continue 
					  */
}

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	int status;

	/* Initialize timer */
	timer_setup(&my_timer, timer_callback, 0);
	/* Initialize completion */
	init_completion(&comp);

	printk("mycompletion - Start the timer the first time. Time: 40ms\n");
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(40)); /* Time expires after 40ms*/
	status = wait_for_completion_timeout(&comp, msecs_to_jiffies(100));
	if(!status)
		printk("mycompletion - Completion timed out!\n"); /* Should not enter here because timer is 40ms and comp timeout is 100*/
	
	reinit_completion(&comp); /* Reuse same completion */

	printk("mycompletion - Start the timer the second time. Time: 400ms\n");
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(400)); /* Time expires after 40ms*/
	status = wait_for_completion_timeout(&comp, msecs_to_jiffies(100));
	if(!status)
		printk("mycompletion - Completion timed out!\n"); /* Should enter here because timer is 400ms and comp timeout is 100*/
	
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	del_timer(&my_timer);
}

module_init(ModuleInit);
module_exit(ModuleExit);


