#include<linux/miscdevice.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/proc_fs.h>
#include<linux/errno.h>
#include<asm/uaccess.h>
#include<linux/slab.h>
#include<linux/sched.h>
#include<linux/types.h>

#define TRAVERSE_COMPLETE -1

static char* output;
struct task_struct *current_p,*next_p=NULL;

int buffer_size = 256;
int process_traverse = -1;


static char* getstate(int state){
		switch(state){
		case TASK_RUNNING:
		{
				return "TASK_RUNNING";
		}
		case TASK_INTERRUPTIBLE:
		{
			return "TASK_INTERRUPTIBLE";
		}
		case TASK_UNINTERRUPTIBLE:
		{
			return "TASK_UNINTERRUPTIBLE";
		}
		case TASK_STOPPED:
		{
			return "TASK_STOPPED";
		}
		case TASK_TRACED:{
			return "TASK_TRACED";
		}
		case TASK_DEAD:{
			return "TASK_DEAD";
		}
		case TASK_WAKEKILL:{
			return "TASK_WAKEKILL";
		}
		}
	return "IDLE";
}
void __exit mydevice_exit(void);
static int mydevice_processlist_open(struct inode *inode,struct file *filep)
{	
	process_traverse = 0;
        pr_info("File open \n");
        return 0;
}
static int mydevice_processlist_close(struct inode *inodep,struct file *filep)
{
	process_traverse = -1;
        pr_info("File close \n");
        return 0;
}

static ssize_t mydevice_processlist_read(struct file *filep, char *buffer, size_t len, loff_t *offset) 
{
	unsigned int cpu;
	int status = 0;
	int length=0;
	struct task_struct *p;
	char *State;	
	if(process_traverse==0)
	{
		p = &init_task;
	}
	else
	{
		p = current_p;
	}
	printk("READ DRIVER CALL\n");
	State = getstate(p->state);
	cpu = task_cpu(p);
	//Process List Over
	if(process_traverse == -1)
	{
		return TRAVERSE_COMPLETE;
	}
	//Iterate over each task 
	length = sprintf(output,"PID=%d \t PPID=%d \t CPU=%d \t STATE=%s"
,p->pid,p->real_parent->pid,cpu,State);
	pr_info("Output buffer details %s \n",output);
	status = copy_to_user(buffer,output,buffer_size);
	if(status !=0)
	{
		pr_err("Failed to copy to user \n");
		return -EINVAL;
	}
	
	process_traverse++;
	current_p = next_task(p);
	if (current_p == &init_task)
	{
		process_traverse =-1;
		next_p = &init_task;
	}
	return buffer_size;
}

static const struct file_operations mydevice_processlist_fops = {
        .owner = THIS_MODULE,
        .open = mydevice_processlist_open,
        .release = mydevice_processlist_close,
	.read = mydevice_processlist_read,
};
static struct miscdevice mydevice_processlist_device = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "process_list",
        .fops = &mydevice_processlist_fops,
};
int __init mydevice_init(void)
{
        int value;
        value = misc_register(&mydevice_processlist_device);
        if(value)
        {
                pr_err("Device can't be register: \n");
                return value;
        }
	output = kmalloc(buffer_size, GFP_KERNEL);
	if(!output){
		value = -ENOMEM;
		goto fail;
	}
	memset(output,0,buffer_size);

        pr_info("Device register \n");
        return 0;
	fail:
	     mydevice_exit();
	     return value;

}
void __exit mydevice_exit(void)
{
        misc_deregister(&mydevice_processlist_device);
	if(output){
		kfree(output);
	}

        pr_info("Device unregister");
}
module_init(mydevice_init);
module_exit(mydevice_exit);
MODULE_DESCRIPTION("Misc Character Driver");
MODULE_AUTHOR("Shubham Gupta <sgupta30@binghamton.edu>");
MODULE_LICENSE("GPL");
