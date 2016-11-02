
#include "sched.h"
#include "debug.h"
#include "task.h"

// 可调度进程链表
task_struct_t *running_proc_head = NULL;

// 等待进程链表
task_struct_t *wait_proc_head = NULL;

// 当前运行的任务
task_struct_t *current;

void init_sched()
{
	printk("task_struct_t size %d\n", sizeof(task_struct_t));
	// 为当前执行流创建信息结构体 该结构位于当前执行流的栈最低端
	//current = (struct task_struct *)(kern_stack_top - 0x2000);
	//current = kmalloc_a(sizeof(task_struct_t));
	current = (task_struct_t *)kmalloc(0x2000);	
	// 0x0010d828
	//printk("current location : 0x%08x\n", &current);
	current->state = TASK_RUNNABLE;
	current->pid = now_pid++;
	current->stack = current; 	// 该成员指向栈低地址
	current->mm = NULL; 		// 内核线程不需要该成员

	// 单向循环链表
	current->next = current;

	running_proc_head = current;
	//schedule();	//将elf文件加载到内存，暂时不执行
}

void kschedule()
{
	if (current) {
		change_task_to(current->next);
	}
}

//存当前任务的执行上下文和切换到下一个任务
void change_task_to(struct task_struct *next)
{
	if (current != next) {
		//printk("task changed");
		struct task_struct *prev = current;
		current = next;
		//printk("prev->context: 0x%08x", &prev->context);
		switch_to(&(prev->context), &(current->context));
	}
}

