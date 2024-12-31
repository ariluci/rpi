#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LA");
MODULE_DESCRIPTION("Demonstartion for dynamic memory management in a LKM");

struct driver_data {
	u8 version;
	char data[64];
};

u32  *ptr1;
struct driver_data *ptr2;

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
		/* Kmalloc - basic alloc */
	ptr1 = kmalloc(sizeof(u32), GFP_KERNEL);
	if(ptr1 == NULL) {
		printk("alloc_test - Out of memory!");
		return -1;
	}
	printk("alloc_test - *ptr1: 0x%x\n", *ptr1);
	*ptr1 = 0xc001c0de;
	printk("alloc_test - *ptr1: 0x%x\n", *ptr1);
	kfree(ptr1);

	/* Kzalloc - Zero init alloc */
	ptr1 = kzalloc(sizeof(u32), GFP_KERNEL);
	if(ptr1 == NULL) {
		printk("alloc_test - Out of memory!");
		return -1;
	}
	printk("alloc_test - *ptr1: 0x%x\n", *ptr1);
	*ptr1 = 0xc001c0de;
	printk("alloc_test - *ptr1: 0x%x\n", *ptr1);
	kfree(ptr1);

	ptr2 = kzalloc(sizeof(struct driver_data), GFP_KERNEL);
	if(ptr2 == NULL){
		printk("alloc_test - Out of memory!");
		return -1;
	}
	ptr2->version = 123;
	strcpy(ptr2->data, "This is a test string for my linux kernel module");

		printk("alloc_test - *ptr2->version: 0x%d\n", ptr2->version);
		printk("alloc_test - *ptr2->data: 0x%s\n", ptr2->data);

	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	printk("alloc_test - ptr2->version: %d\n", ptr2->version);
	printk("alloc_test - ptr2->data: %s\n", ptr2->data);
	kfree(ptr2);
}

module_init(ModuleInit);
module_exit(ModuleExit);


