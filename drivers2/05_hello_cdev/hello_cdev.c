#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

static int major;

static ssize_t my_read(struct file *f, char __user *u, size_t l, loff_t *o)
{
	printk("hello_cdev - Read is called \n");
	return 0;
}

static struct file_operations fops = {
	.read = my_read
};

static int __init my_init(void) 
{
	major = register_chrdev(0, "hello_cdev", &fops); /*0 will search and select a free device number,  we can hardcote it to a number*/
	if (major < 0) {
		printk("hello_cdev - Error registering chrdev\n");
		return major;
	}
	printk("hello_cdev - Major device number: %d\n", major);	
	return 0;
}

static void __exit my_exit(void) {
	unregister_chrdev(major, "hello_cdev");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lucian");
MODULE_DESCRIPTION("A simple driver for registering a character device");

/*
Steps:
 - Allocate a device number major and minor(minor dev nums range from 0 to 255)
 - create chardevice
 - bind chardevice to device number
*/
