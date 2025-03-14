#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/slab.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("LA");
MODULE_DESCRIPTION("An example for using private data for file descriptors");

#define DEVNR 64
#define DEVNRNAME "my_cdev"

static struct cdev my_cdev;
static int my_open(struct inode *inode, struct file *file) {
	char *ptr;
	ptr = kzalloc(256, GFP_KERNEL);
	if (!ptr)
		return -ENOMEM;
	printk("priv_data - Allocated 256 bytes of memory\n");
	file->private_data  = ptr;
	return 0;
}

static ssize_t my_read(struct file *file, char __user *user_buffer, size_t len, loff_t *off) {
	char *buffer;
	int not_copied, to_copy = (len < 256) ? len : 256;

	buffer = (char*)file->private_data;
	if(!buffer)
		return -1;

	printk("priv_data - my_read called, *off:  %lld\n", *off);
	if(*off >= to_copy)
		return 0;
	
	not_copied = copy_to_user(user_buffer, buffer, to_copy);
	
	*off += to_copy - not_copied;

	return to_copy - not_copied;
}


static ssize_t my_write(struct file *file, const char __user *user_buffer, size_t len, loff_t *off) {
	char *buffer;
	int not_copied, to_copy = (len < 256) ? len : 256;
	
	buffer = (char *)file->private_data;
	if(!buffer)
		return -1;
	
	printk("priv_data - my_write called\n");	
	not_copied = copy_from_user(buffer, user_buffer, to_copy);
	return to_copy - not_copied;
}

static int my_close(struct inode *inode, struct file *file)
{
	char *ptr;
	ptr = (char *)file->private_data;
	if(ptr)
		kfree(ptr);
	printk("priv_data -freed 256 bytes\n");
	return 0;
}
struct file_operations fops = {
	.open = my_open,
	.release = my_close,
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
		printk("priv_data - Error registering device number!\n");
		return status;
	}

	cdev_init(&my_cdev, &fops);
	my_cdev.owner = THIS_MODULE;

	status = cdev_add(&my_cdev, devnr,1);
	if (status < 0) {
		printk("priv_data - Error adding cdev\n");
		unregister_chrdev_region(devnr,1);
		return status;
	}
	printk("priv_data - Registered device nr 64 0, created cdev\n");	
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	dev_t devnr = MKDEV(DEVNR,0);
	unregister_chrdev_region(devnr, 1);
	cdev_del(&my_cdev);
	printk("priv_data - Unregister gistered device nr 64 0\n");	
}

module_init(ModuleInit);
module_exit(ModuleExit);

