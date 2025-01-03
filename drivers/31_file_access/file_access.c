/* It's better to manage files from userspace not kernel drivers 
 * This is however usefull for example if you have a device 
 * and the driver has to load firmware that changes ofter into the device. 
 * In this example it is prefered to do it via driver than to recomple the driver
 */ 
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NA");
MODULE_DESCRIPTION("An example of reading and writeg to a filesystem from a driver");

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	struct file *file;
	char data[128] = "\"Hey, there's Skittles in there!\"\n";
	ssize_t len;

	/* Open the file */
	file = filp_open("/tmp/dude", O_RDWR | O_CREAT, 0644);
	if(!file) {
		printk("file_access - Error opening file\n");
		return -1;
	}
	/* Write to the file */
	len = kernel_write(file, data, sizeof(data), &file->f_pos);
	if(len < 0){
		printk("file_access - Error writing to file: %d\n", len);
		filp_close(file, NULL);
	}
	printk("file_access - Wrote %d bytes to file\n", len);
	
	/* Read it back */
	memset(data, 0, sizeof(data));
	file->f_pos = 0;

	len = kernel_read(file, data, sizeof(data), &file->f_pos);
		if(len < 0){
		printk("file_access - Error reading file: %d \n", len);
		filp_close(file, NULL);
	}

	printk("file_access - Read %d bytes: %s\n", len, data);
	filp_close(file, NULL);
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	printk("file_access - Unloading driver\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

