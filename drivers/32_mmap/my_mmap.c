/*
We will pas a driver to the dinamically allocated memory
referenced by my_data of mmap to user space.
From user space we can directly access it.
*/
/* Address format : b15-b4 - page frame nr.
                    b3-b0  - page offset */
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/page.h>
#include <asm/io.h> 

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("LA");
MODULE_DESCRIPTION("A implementation of mmap syscall");

#define MYMAJOR 64
#define DEVNAME "mydev"

static void *my_data;

static ssize_t my_read(struct file *file, char __user *user_buffer, size_t len, loff_t *offs) {
    int not_copied, to_copy = (len > PAGE_SIZE) ? PAGE_SIZE : len;
    not_copied = copy_to_user(user_buffer, my_data, to_copy);
    return to_copy - not_copied;
}

static ssize_t my_write(struct file *file, const char __user *user_buffer, size_t len, loff_t *offs) {
    int not_copied, to_copy = (len > PAGE_SIZE) ? PAGE_SIZE : len;
    not_copied = copy_from_user(my_data, user_buffer, to_copy);
    return to_copy - not_copied;
}

/* get a vma from a process and remap it to poit to my_data*/
static int my_mmap(struct file *file, struct vm_area_struct *vma)
{
    int status;
    unsigned long vma_size;
	
    vma->vm_pgoff = virt_to_phys(my_data) >> PAGE_SHIFT; /* Set the physicial adress to which this regios should be mapped */
                                                         /* We need to converd virtual add to phys */
                                                         /* shift to get only offset from address */
    /*Do the remap */
    vma_size = vma->vm_end - vma->vm_start;
    status = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, vma_size, vma->vm_page_prot); 
	if(status) {
		printk("my_mmap - Error remap_pfn_range: %d\n", status);
		return -EAGAIN;
	}
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
    .mmap = my_mmap,
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
    int status;

	printk("my_mmap - Hello!\n");	

    my_data = kzalloc(PAGE_SIZE, GFP_DMA); /* MMU translates physical address to virtual address and vice versa */
                                           /* With virtual addresses every process has its own address space */
                                           /* The smallest allocatable size a mmu can address is a PAGE */
                                           /* On linux usually a page is 4KB*/
                                           /*GFP_KERNEL -> virtual address space is NOT continuous in physical address space
                                             GFP_DMA    -> virtual address space is continuous in physical address space */                                
    if(!my_data)
        return -ENOMEM;
    
    printk("my_mmap - I have allocated a page (%ld Bytes)\n", PAGE_SIZE);
    printk("my_mmap - PAGESHIFT: %d\n", PAGE_SHIFT);
    
    status = register_chrdev(MYMAJOR, DEVNAME, &fops);
    if(status < 0) {
        printk("my_mmap - Error registering device number!\n");
        kfree(my_data);
        return status;
    }
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	if(my_data)
		kfree(my_data);
	unregister_chrdev(MYMAJOR, DEVNAME);	
	printk("my_mmap - Goodbye\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

