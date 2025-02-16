#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

static int major;

/*with inode we can get dev number*/
static int my_open(struct inode *inode, struct file *filp)
{
	pr_info("hello_cdev - Major: %d, Minor %d\n", imajor(inode), iminor(inode));
	pr_info("hello_cdev - filp->f_pos :  %lld\n", filp->f_pos);
	pr_info("hello_cdev - filp->f_mode : 0x%x\n", filp->f_mode);
	pr_info("hello_cdev - filp->f_flags: 0x%x\n", filp->f_flags);

	return 0;
}

static int my_release(struct inode *inode, struct file *filp){
	pr_info("hello_cdev - File is closed\n");
	return 0;
}

static struct file_operations fops = {
	.open = my_open,
	.release = my_release,
};

static int __init my_init(void) 
{
	major = register_chrdev(0, "hello_cdev", &fops); /*0 will search and select a free device number,  we can hardcote it to a number*/
	if (major < 0) {
		pr_err("hello_cdev - Error registering chrdev\n");
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
