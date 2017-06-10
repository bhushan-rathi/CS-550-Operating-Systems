//kernel code libraries
#include <linux/init.h>
#include <linux/module.h> // for module
#include <linux/fs.h> // for char driver 
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/miscdevice.h>
#include <linux/err.h>
#include <linux/kdev_t.h>
#include <linux/wait.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>

//#include <linux/stdlib.h>
//module license
MODULE_LICENSE("GPL");

int var;
module_param(var, int, 0);

//global varibles

static DEFINE_SEMAPHORE(full);

static DEFINE_SEMAPHORE(empty);

static DEFINE_MUTEX(mutVar);

char **item;
int front=-1;
int rear=-1;

//to perform read operation
ssize_t read(struct file *pnode, char __user *buffer, size_t length, loff_t *offset)
{
	int e1 = 1;
	int e6 = 1;
	int e7 = 1;
	int r = 1;

	//taking lock
	e6 = down_interruptible(&full);
	if(e6 < 0){printk(KERN_ALERT "In down_interruptible(&full) %d",e6);return e6;}

	e7 = mutex_lock_interruptible(&mutVar);
	if(e7 < 0){
		printk(KERN_ALERT "In mutex read func. %d", e7);
		up(&empty);
		return e7;}


	//critical section
		e1 = copy_to_user(buffer, item[front], strlen(item[front])+1);
		if(e1<0){
			printk(KERN_ALERT "In with copy to user %d", e1);
			//removing lock			
			mutex_unlock(&mutVar);
			up(&empty);
			return e1;}
		
		r = strlen(item[front]);
		//printk(KERN_ALERT "front = %d", front);
		kfree (item[front]);
		front = (front+1)%var;
	
	
	//removing lock
	mutex_unlock(&mutVar);
	up(&empty);

	return r;
}

//to perform write operation
ssize_t write(struct file *pnode, const char __user *buffer, size_t length, loff_t *offset)
{
	int e3 = 1;
	int e4 = 1;
	int e5 = 1;

	//taking lock	
	e4 = down_interruptible(&empty);
	if(e4 < 0){printk(KERN_ALERT "In down_interruptible(&empty) %d", e4);return e4;}

	e5 = mutex_lock_interruptible(&mutVar);
	if(e5 < 0){
		printk(KERN_ALERT "In mutex_lock write func. %d", e5);
		up(&full);
		return e5;}

	//critical section
		if(front == -1){front = 0;}
		rear = (rear+1)%var;
		item[rear] = kmalloc ( length, GFP_KERNEL);

		if(item[rear]<0){
			printk(KERN_ALERT "In malloc write");
			//removing lock
			mutex_unlock(&mutVar);
			up(&full);
			return -1;}

		e3 = copy_from_user(item[rear], buffer, length);
			if(e3<0){
			printk(KERN_ALERT "In copy from user %d", e3);
			kfree(item[rear]);
			rear = rear - 1;	
			//removing lock
			mutex_unlock(&mutVar);
			up(&full);			
			return 0;}
	
		//printk(KERN_ALERT "rear = %d", rear);
	
	//removing lock
	mutex_unlock(&mutVar);
	up(&full);

	return strlen(item[rear]);
}
//to open device
int open(struct inode *inode, struct file *pnode){return 0;}

//to close the device operation
int close(struct inode *inode, struct file *pnode){return 0;}


//to hold file operations
static struct file_operations my_fops={		
		.open = open,
		.release = close,
		.read = read,
		.write = write,
	};
//keeping device info
 static struct miscdevice mydevice={
.minor=MISC_DYNAMIC_MINOR,
.name ="linepipe",
.fops=&my_fops
};
//to register kernel
	static int  linepipe_init(void)
	{
		int e2 = 1;
		item = (char **)kmalloc(var * sizeof(char *), GFP_KERNEL);
		if(item<0){printk(KERN_ALERT "In malloc init");return -1;}
		sema_init(&full, 0);
		sema_init(&empty, var);	
		e2 = misc_register(&mydevice);
		if(e2<0){printk(KERN_ALERT " kernel registration failed %d", e2);return e2;}
		return 0;
	}
//to deregister kernel
	static void  linepipe_exit(void)
	{
		kfree(item);		
		misc_deregister(&mydevice);
	}
module_init(linepipe_init);
module_exit(linepipe_exit);
