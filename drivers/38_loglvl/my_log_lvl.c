#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NA");
MODULE_DESCRIPTION("A hello world LKM");

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	printk(KERN_DEBUG "This is a debug msg\n");
	printk(KERN_INFO "Info: %d\n", 42);
	printk(KERN_ERR "ERROR!!!\n");
	printk(KERN_ALERT "ALERT!!!\n");
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	/* Aliases for printk with log lvl params */
	pr_info("Another Info\n"); 
	pr_crit("Critical info: %d\n", 123);
	pr_err("Second error\n");
}


module_init(ModuleInit);
module_exit(ModuleExit);

