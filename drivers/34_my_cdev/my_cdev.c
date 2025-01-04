/* Alternative method to legacy using register_chdev() */
/* This method allows to create multiple devices at the same time */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("LA");
MODULE_DESCRIPTION("An example for registering device numbers for character devices");

#define DEVNR 64
#define DEVNRNAME "my_cdev"

static struct cdev my_cdev;
static char buffer[256];

static ssize_t my_read(struct file *file, char __user *user_buffer, size_t len, loff_t *off) {
	int not_copied, to_copy = (len < 256) ? len : 256;
	printk("my_cdev - my_read called, *off:  %lld\n", *off);
	if(*off >= to_copy)
		return 0;
	
	not_copied = copy_to_user(user_buffer, buffer, to_copy);
	
	*off += to_copy - not_copied;

	return to_copy - not_copied;
}


static ssize_t my_write(struct file *file, const char __user *user_buffer, size_t len, loff_t *off) {
	int not_copied, to_copy = (len < 256) ? len : 256;
	printk("my_cdev - my_write called\n");	
	not_copied = copy_from_user(buffer, user_buffer, to_copy);
	return to_copy - not_copied;
}


struct file_operations fops = {
	.read = my_read,
	.write = my_write,
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	//register_chdev(64, "my_cdev", &fops) /* Previous method */
	int status;
	dev_t devnr = MKDEV(DEVNR, 0);
	status = register_chrdev_region(devnr, 1, DEVNRNAME); /* just 1 device*/
	if(status < 0) {
		printk("my_cdev - Error registering device number!\n");
		return status;
	}

	cdev_init(&my_cdev, &fops);
	my_cdev.owner = THIS_MODULE;

	status = cdev_add(&my_cdev, devnr,1);
	if (status < 0) {
		printk("my_cdev - Error adding cdev\n");
		unregister_chrdev_region(devnr,1);
		return status;
	}
	printk("my_cdev - Registered device nr 64 0, created cdev\n");	
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	dev_t devnr = MKDEV(DEVNR,0);
	unregister_chrdev_region(devnr, 1);
	cdev_del(&my_cdev);
	printk("my_cdev - Unregister gistered device nr 64 0\n");	
}

module_init(ModuleInit);
module_exit(ModuleExit);

