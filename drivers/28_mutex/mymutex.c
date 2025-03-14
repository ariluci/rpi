#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/mutex.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("LA");
MODULE_DESCRIPTION("Thread mutex driver");

/* Global variables for threads */
static struct task_struct *kthread_1;
static struct task_struct *kthread_2;
static int t1 = 1, t2 = 2;
static struct mutex lock;

/**
 * @brief Function will be executed by the thread
 * @param thread_nr : pointer to the number of the thread 
 */
int thread_function(void *thread_nr) {
    int delay[] = {0, 1000, 500};
    int t_nr = *(int *)thread_nr;

    printk("mymutex - Thread %d is executed!\n", t_nr);
    
    mutex_lock(&lock);
    printk("mymutex - Thread %d is in the critical section!\n", t_nr);
    msleep(delay[t_nr]);
    printk("mymutex - Thread %d is leaving the critical section!\n", t_nr);
    mutex_unlock(&lock);
    
    printk("mymutex - Thread %d finished execution!\n", t_nr);
    return 0;
}

/**
 * @brief This function is called when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
    printk("mymutex - Init threads\n");
    
    mutex_init(&lock);

    /* Method 1 for creating threads */
    kthread_1 = kthread_create(thread_function, &t1, "kthread_1");
    if(kthread_1 != NULL) {
        /* Start thread */
        wake_up_process(kthread_1);
        printk("mymutex - Thread 1 was created and is running now\n");
    }
    else {
        printk("mymutex - Thread 1 could not be created!\n");
        return -1;
    }

    /* Method 2 for creating threads */
    kthread_2 = kthread_run(thread_function, &t2, "kthread_2");
    if(kthread_2 != NULL) {
          printk("mymutex - Thread 2 was created and is running now\n");
    }
    else {
        printk("mymutex - Thread 2 could not be created!\n");
        kthread_stop(kthread_1);
        return -1;
    }
    printk("mymutex - Both threads are running now! \n");

    return 0;
}
/**
 * @brief This Function is called when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
    printk("mymutex - Stop both threads\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

