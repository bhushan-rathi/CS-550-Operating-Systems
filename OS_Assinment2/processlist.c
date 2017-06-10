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
//#include <linux/stdlib.h>
//module license
MODULE_LICENSE("GPL");

//global varibles
char *msg;
static struct task_struct *line;
char statemsg[350];
int e1=1;

//to perform read operation
ssize_t read(struct file *pnode, char __user *buffer, size_t length, loff_t *offset)
{

		if(line == &init_task)
		{
			return 0;	
		}
		else{  

			msg=(char *)kmalloc(350*sizeof(char),GFP_KERNEL);
			memset(msg,'\0',350);
			
			if(line->state == 0){
				strcpy(statemsg, "TASK_RUNNING");
			}
			if(line->state == 1){
				strcpy(statemsg, "TASK_INTERRUPTIBLE");
			}
			if(line->state == 2){
				strcpy(statemsg, "TASK_UNINTERRUPTIBLE");
			}
			if(line->state == 4){
				strcpy(statemsg, "__TASK_STOPPED");
			}
			if(line->state == 8){
				strcpy(statemsg, "__TASK_TRACED");
			}
			if(line->state == 16){
				strcpy(statemsg, "EXIT_DEAD");
			}
			if(line->state == 32){
				strcpy(statemsg, "EXIT_ZOMBIE");
			}
			if(line->state == 64){
				strcpy(statemsg, "EXIT_DEAD");
			}
			if(line->state == 128){
				strcpy(statemsg, "TASK_WAKEKILL");
			}
			if(line->state == 256){
				strcpy(statemsg, "TASK_WAKING");
			}
			if(line->state == 1024){
				strcpy(statemsg, "TASK_NOLOAD");
			}
			if(line->state == 512){
				strcpy(statemsg, "TASK_PARKED");
			}
			if(line->state == 1024){
				strcpy(statemsg, "TASK_NOLOAD");
			}
			if(line->state == 2048){
				strcpy(statemsg, "TASK_NEW");
			}
			if(line->state == 4096){
				strcpy(statemsg, "TASK_STATE_MAX");
			}	
			sprintf(msg, "PID=%d PPID=%d CPU =%d STATE=%s",line->pid,line->parent->pid,task_cpu(line),statemsg);
				
			e1 = copy_to_user(buffer, msg, strlen(msg)+1);			
			if(e1<0){printk(KERN_ALERT "issue with copy to user");return 0;}			
			line=next_task(line);			
			kfree(msg);
			return sizeof(buffer)-1;
		}
}
//to open device
int open(struct inode *inode, struct file *pnode)
{
		line=&init_task;
		line=next_task(line);
		return 0;
}
//to close the device operation
int close(struct inode *inode, struct file *pnode)
{
return 0;
}


//to hold file operations
static struct file_operations my_fops={		
		.open = open,
		.release = close,
		.read = read,
	};
//keeping device info
 static struct miscdevice mydevice={
.minor=MISC_DYNAMIC_MINOR,
.name ="process_list",
.fops=&my_fops
};
//to register kernel
	static int  processlist_init(void)
	{
		int e2=1;
		e2 = misc_register(&mydevice);
		if(e2<0){printk(KERN_ALERT "issue with ckernel registration");return 0;}
		return 0;
	}
////to deregister kernel
	static void  processlist_exit(void)
	{
		misc_deregister(&mydevice);
	}
module_init(processlist_init);
module_exit(processlist_exit);
