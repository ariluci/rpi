#include <linux/module.h>
#include <linux/init.h>

static int __init my_init(void) {
	printk(KERN_INFO "log_level - This is an info!\n");
	printk(KERN_WARNING "log_level - This is a warning!\n");	
	printk(KERN_ALERT "log_level - This is an alert!\n");	
	printk(KERN_DEBUG "log_level - Debug %d\n", 42);	
	return 0;
}

static void __exit my_exit(void) {
	pr_emerg("log_level - module is unloaded\n");
	pr_err("log_level - ERROR\n");	
	pr_info("log_level - Last info...\n");	
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lucian");
MODULE_DESCRIPTION("A simple Hello World LKM");

/* static to keep functions only usable by this module */
/* __init __exit dont' do anything; they just improve readability */ 