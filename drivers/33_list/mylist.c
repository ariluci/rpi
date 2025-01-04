#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h> /* For kzalloc */

/* Meta Info */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("LA");
MODULE_DESCRIPTION("A linked list LKM example");

struct my_data {
	struct list_head list;
	char text[64];
};

LIST_HEAD(my_list);

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	struct my_data *tmp, *next;
	struct list_head *ptr;
	printk("mylist - Hello, Kernel!\n");	
	
	/* Let's create a list with three elements */
	tmp = kmalloc(sizeof(struct my_data), GFP_KERNEL);
	strcpy(tmp->text, "First Element");
	list_add_tail(&tmp->list, &my_list);

	tmp = kmalloc(sizeof(struct my_data), GFP_KERNEL);
	strcpy(tmp->text, "Second Element");
	list_add_tail(&tmp->list, &my_list);

	tmp = kmalloc(sizeof(struct my_data), GFP_KERNEL);
	strcpy(&tmp->text, "Third Element");
	list_add_tail(&tmp->list, &my_list);

	list_for_each(ptr, &my_list) {
	// list_for_each_prev(ptr, &my_list) {  /* To list the other way around*/
		tmp = list_entry(ptr, struct my_data, list);
		printk("mylist - Element text: %s\n", tmp->text);
	}

	/* Free memory */
	list_for_each_entry_safe(tmp, next, &my_list, list) {
		list_del(&tmp->list);
		kfree(tmp);
	}
	printk("mylist -Freeing memory done!\n");

	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	printk("mylist - Goodbye, Kernel!\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

