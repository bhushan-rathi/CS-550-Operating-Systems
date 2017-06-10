
// importing librabies

	#include <linux/kprobes.h>
	#include <linux/kernel.h>
	#include <linux/module.h>


MODULE_LICENSE("GPL");

// declaring varibales

	int var;
	int count;
	struct timespec t1;
	struct timespec t2;
	long t4,t5;
	double t6;
	module_param(var, int, 0);
	

//  handle_mm_fault() routine which will trap pagefaults
static long jhandle_mm_fault(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, unsigned int flags)
{
	if (current->pid == var)
	{
		count++;
		t2=current_kernel_time();
		t4 =  t2.tv_nsec - t1.tv_nsec;
 
		t5 =  t2.tv_sec - t1.tv_sec;
		t5 = t5*1000000000;
		t5 = t5 + t4;
		printk(KERN_ALERT "page fault: access address 0x%p of task(PID:%d, Name:%s at time:%ld nsec  pf no.= %d",
		      address, current->pid, current->comm, t5, count);
		//printk(KERN_ALERT "%ld %lu\n",
		  //     t5, address);
	}

// resuming execution by putting all states bake to execution

	jprobe_return();

	return 0;
}

static struct jprobe my_jprobe = {

	.entry	= jhandle_mm_fault,

	.kp =
		{
		.symbol_name = "handle_mm_fault",
		},
};

static int __init jprobe_init(void)
{
	int ret;
	count = 0;
	t4 = 0;
	t5 = 0;
	t6 = 0;
	t1 = current_kernel_time();
	t2 = current_kernel_time();
	ret = register_jprobe(&my_jprobe);
	if (ret < 0) 
	{
		pr_err("register_jprobe failed, returned %d\n", ret);
		return -1;
	}

	pr_info("Planted jprobe at %p, handler addr %p\n",
	       my_jprobe.kp.addr, my_jprobe.entry);

	return 0;
}

static void __exit jprobe_exit(void)
{
	unregister_jprobe(&my_jprobe);

	pr_info("jprobe at %p unregistered\n", my_jprobe.kp.addr);
}

module_init(jprobe_init)

module_exit(jprobe_exit)


