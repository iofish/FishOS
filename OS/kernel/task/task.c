//2016年05月10日20:18:53
//开始进程了

#include "task.h"
#include "sched.h"
#include "debug.h"
#include "pmm.h"
#include "global.h"
// 当前进程
volatile task_t *current_task; 
//多任务环境下各任务间共享的标志应该加volatile
// 进程队列
volatile task_t *ready_queue;
volatile fish_task_t *task_queue = NULL;
volatile fish_task_t *current_ftask = NULL;
// extern一些变量
extern page_directory_t *kernel_direcotry;
extern page_directory_t *current_directory;
extern void alloc_frame(page_t *, int ,int );
extern uint32_t initial_esp;
extern uint32_t read_eip();
int32_t now_pid = 0;
int32_t nTasks = 0;
int32_t nfTasks = 0;
extern int32_t task_array[3];
uint32_t process_to_run[3];

// tss选择子
int32_t tssNum = 5;
tss_entry_t *tss_entry;
extern ldsymbol ld_tss_stack;
// 内核线程创建
int32_t kernel_thread(int (*fn)(void *), void *arg)
{
	task_struct_t *new_task = kmalloc_a(sizeof(0x2000));
	printk("new task address: 0x%08x\n", new_task);
	// 0x00110d24
	new_task -> state = TASK_RUNNABLE;
	new_task -> stack = current;
	new_task -> pid = now_pid++;
	new_task -> mm = NULL;

	uint32_t *stack_top = (uint32_t *)((uint32_t)new_task + 0x2000);
	printk("new task stack top: 0x%08x\n", stack_top);
	*(--stack_top) = (uint32_t)arg; // 线程变量的入口地址
	*(--stack_top) = (uint32_t)kthread_exit;  // 线程退出返回地址
	*(--stack_top) = (uint32_t)fn;	// 程序的入口地址

	new_task->context.esp = (uint32_t)new_task + 0x2000 - sizeof(uint32_t) * 3;
	printk("new_task -> context.esp : 0x%08x\n", &new_task->context.esp);
	printk("new_task -> context.ebp : 0x%08x\n", &new_task->context.ebp);
	printk("new_task -> arg : 0x%08x\n", arg);
	printk("new_task -> kthread_exit : 0x%08x\n", &kthread_exit);
	printk("new_task -> fn : 0x%08x\n", fn);
	// 设置新任务的标志寄存器未屏蔽中断，很重要
	new_task->context.eflags = 0x200;
	new_task->next = running_proc_head;
	
	// 找到当前进任务队列，插入到末尾
	struct task_struct *tail = running_proc_head;

	while (tail->next != running_proc_head) {
		tail = tail->next;
	}
	tail->next = new_task;

	return new_task->pid;
}

void kthread_exit()
{
	register uint32_t val asm ("eax");	//函数的返回值

	//printk("Thread exited with value %d\n", val);

	while (1);
}

//下一个可用的id
uint32_t next_pid = 1;

void init_tasking()
{
	// 关中断
	asm volatile("cli");
	// 分配一个进程大小的地址空间
	//printk("%d", sizeof(task_t));
	// 2016.7.3
	uint32_t *stack;
	task_t *task = (task_t *)kmalloc(sizeof(task_t));
	current_task = task;	
	ready_queue =  task; //(task_t *)kmalloc(sizeof(task_t)); // task_t 24字节
	task -> stack = kmalloc(0x1000) + 0x1000;			 // 分配4KB空间
	task -> esp = task -> stack;
	task -> originalStack = task -> stack - 0x1000;			// 在开始位置设置原始栈

	task -> page_directory = current_directory;
	task -> priority = PRIO_LOW;
	task -> time_to_run = 10;
	task -> time_running = 0;
	task -> ready_to_run = TRUE;

	task -> burst_time = 0;
	task -> averaged_priority = 0;
	
	nTasks++;

	stack = (uint32_t *)task -> stack;

	// 处理器数据 iret
	*--stack = 0x202;	// EFLAGS
	*--stack = 0x08;	// CS
	*--stack = 0;		// EIP
	task -> eip = 0;
	// pusha
	*--stack = 0;		// EDI
	*--stack = 0;		// ESI
	*--stack = 0;		// EBP
	
	task -> ebp = 0;	
	*--stack = 0;		// NULL
	*--stack = 0;		// EBX
	*--stack = 0;		// EDX
	*--stack = 0;		// ECX
	*--stack = 0;		// EAX
	// 分段寄存器
	*--stack = 0x10;	// DS
	*--stack = 0x10;	// ES
	*--stack = 0x10;	// FS
	*--stack = 0x10;	// GS

	task -> id = next_pid ++;

	task -> stack = (uint32_t)stack;

	task -> next = 0;

//	printk("current task 0x%08x\n", current_task);
	//current_task -> id = next_pid ++;
	//current_task -> esp = current_task -> ebp = 0;
	//current_task -> eip = 0;
	//current_task -> page_directory = current_directory;
	//printk("process initial directory 0x%08x\n", current_directory);
	// 页目录是init_paging()形成的目录, 0x00119000	
	//current_task -> next = 0;
	// 开中断
	asm volatile("sti");
}




int32_t nfork(uint32_t priority, uint32_t burst_time, char *task_Name)
{
	asm volatile("cli");
	
	static task_t * parent_task;
	parent_task = (task_t *)current_task;

	uint32_t id = next_pid ++;
	uint32_t *stack;
	task_t *task = (task_t *)kmalloc(sizeof(task_t));
	task -> stack = kmalloc(0x1000) - 0x1000;
	task -> esp = task -> stack;
	task -> originalStack = task -> stack - 0x1000;

	page_directory_t *directory = clone_directory(current_directory);
	task -> page_directory = directory;

	task -> priority = priority;
	task -> time_to_run = 0;
	task -> time_running = 0;
	task -> ready_to_run = TRUE;

	task -> burst_time = burst_time;
	task -> averaged_priority = priority + burst_time;

	nTasks++;

	stack = (uint32_t*)task -> stack;
	// iret
	*--stack = 0x202;	// EFLAGS
	*--stack = 0x08;	// CS
	*--stack = 0;		// EIP
	
	// pusha
	*--stack = 0;		// EDI
	*--stack = 0;		// ESI
	*--stack = 0;		// EBP
	task -> ebp = 0;
	*--stack = 0;		// NULL
	*--stack = 0;		// EBX
	*--stack = 0;		// EDX
	*--stack = 0;		// ECX
	*--stack = 0;		// EAX
	
	// date segment
	*--stack = 0x10;	// DS
	*--stack = 0x10;	// ES
	*--stack = 0x10;	// FS
	*--stack = 0x10;	// GS

	task -> id = id;
	task -> stack = (uint32_t)stack;
	task -> thread = 0;	
	task -> thread_flags = 0;

	task -> next = 0;
	uint32_t eip = read_eip();
	if(current_task == parent_task) // child
	{
		uint32_t esp; asm volatile("mov %%esp, %0" : "=r"(esp));
		uint32_t ebp; asm volatile("mov %%ebp, %0" : "=r"(ebp));

		task -> esp = esp;
		task -> ebp = ebp;
		task -> eip = eip;

		asm volatile("sti");
		return task -> id;
	}else{
		return 0;
	}
	return id;
}

// fork()函数需要干啥子嘞？
int fork()
{
	// 需要改变原有的数据，需要关中断
	asm volatile("cli");
	// 父进程
	task_t *parent_task = (task_t *)current_task;
	// 需要拷贝父进程的地址空间
	page_directory_t *directory = clone_directory(current_directory);
	// 然后创建一个新的进程
	task_t *new_task = (task_t *)kmalloc(sizeof(task_t));
	printk("new task location 0x%08x\n", new_task);
	new_task -> id = next_pid ++;
	new_task -> esp = new_task -> ebp = 0;
	new_task -> page_directory = directory;
	printk("new process directory 0x%08x\n", directory);
	new_task -> next = 0;

	// 将新进程写入到进程队列最后面
	task_t *tmp_task = (task_t *)ready_queue;
	while(tmp_task -> next)
		tmp_task = tmp_task ->next;
	tmp_task -> next = new_task;

	// ip指向哪儿？ eip 指向栈中的eax
	// 获取eip之后，让进程的下一个指令指向eip
	uint32_t eip = read_eip();
	printk("read_eip values 0x%08x\n", eip);
	// 检查是不是父进程
	if(current_task == parent_task)
	{	//是父进程，设置子进程的esp, ebp, eip
		uint32_t esp, ebp;
		asm volatile("mov %%esp, %0" : "=r"(esp));
		asm volatile("mov %%ebp, %0" : "=r"(ebp));
		new_task -> esp = esp;	
		new_task -> ebp = ebp;
		new_task -> eip = eip;
		current_task = new_task;
		asm volatile("sti");
		return new_task -> id;
	}else{
		//是子进程
		return 0; 
	}	
}

void set_current_task(task_t *task_to_set)
{

  if (current_task == 0)
    return;

  asm volatile("cli");

  if(!task_to_set)
  {
    asm volatile("sti");
    
    return;
  }

  uint32_t esp, ebp, eip;
  asm volatile("mov %%esp, %0" : "=r"(esp));
  asm volatile("mov %%ebp, %0" : "=r"(ebp));

  eip = read_eip();

  if (eip == 0x12345)
    return;
  current_task->eip = eip;
  current_task->esp = esp;
  current_task->ebp = ebp;
  
  current_task = task_to_set;

  eip = current_task->eip;
  esp = current_task->esp;
  ebp = current_task->ebp;

  if(current_task->page_directory)
  {
    current_directory = current_task->page_directory;
  }


  asm volatile(" \
		cli; \
		mov %0, %%ecx; \
		mov %1, %%esp; \
		mov %2, %%ebp; \
		mov %3, %%cr3; \
		mov $0x12345, %%eax; \
		sti; \
		jmp *%%ecx "
               : : "r"(eip), "r"(esp), "r"(ebp), "r"(current_directory->physicalAddr));

  asm volatile("sti");

}


void switch_task()
{
	// 如果没有初始化
	if(!current_task)
		return;
	uint32_t esp, ebp, eip;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp));
	// 读取指令的指针
	// 有两个方式：
	// 1. 返回的是EIP 2. 刚刚进行任务切换，在函数read_eip()之后存储的EIP，就像是刚刚返回的
	// 2表示需要立即返回，这里在程序末尾，给EAX赋一个虚拟的值
	eip = read_eip();
	if(eip == 0x12345)
		return;
	//开始切换任务,寄存器赋值
	current_task -> eip = eip;
	current_task -> esp = esp;
	current_task -> ebp = ebp;
	//指向下一个任务
	current_task = current_task -> next;
	//出现bug
	if(!current_task) current_task = ready_queue;
	
	eip = current_task -> eip;
	esp = current_task -> esp;
	ebp = current_task -> ebp;
	//换页
	if(current_task -> page_directory)
	{	
		current_directory = current_task -> page_directory;
	}	
	//开始，需要关掉中断
	// 将eip的地址写入到ecx中
	// 加载新的结构体中的esp， ebp的值，新的页目录的物理地址加载到CR3
	// 写一个虚拟的EAX值 0x12345来判断是否任务转换成功
	// 打开中断，跳转到ECX地方运行（新的EIP）

   	 asm volatile("         \
      	      	cli;                 \
	      	mov %0, %%ecx;       \
      		mov %1, %%esp;       \
      		mov %2, %%ebp;       \
     		mov %3, %%cr3;       \
      		mov $0x12345, %%eax; \
      		sti;                 \
	      	jmp *%%ecx           "
                 : : "r"(eip), "r"(esp), "r"(ebp),"r"(current_directory->physicalAddr));
}


int getpid()
{
	return current_task -> id;
}

void set_esp0(uint32_t new)
{
    tss_entry -> esp0 = new;
}

void init_fishtask()
{
	/*write on 2016-8-24  */ 	
	asm volatile("cli");
	uint32_t *stack;
	fish_task_t *fishTask = kmalloc(sizeof(fish_task_t));
	current_ftask = fishTask;
	task_queue = fishTask;			// ftask 进程队列
	fishTask -> priority = PRIO_IDLE;		// 进程优先级
	fishTask -> start_time = 0;			
	fishTask -> end_time = 0;	
	nfTasks ++;
	//uint32_t eip = read_eip();
	//fishTask -> tss.eip = eip;
	//fishTask -> tss.eflags = 0x202;	

	fishTask -> task_id = next_pid ++;
	fishTask -> next = NULL;			// 进程链表	
	asm volatile("sti");
}


int32_t fish_Fork(uint32_t priority, char *task_Name)
{
	uint32_t processEntry;
	asm volatile("cli");
	int taskNum = task_Name - 'A';
	switch(taskNum){
		case 0:
			processEntry = load_elf(5000, 40);
			break;
		case 1:
			processEntry = load_elf(5500, 15);
			break;
		case 2:
			processEntry = load_elf(6000, 15);
			break;
		case 3:
			processEntry = load_elf(6500, 15);
			break;
		case 4:
			processEntry = load_elf(7000, 15);
			break;
		case 5:
			processEntry = load_elf(7500, 15);
			break;
		case 6:
			processEntry = load_elf(8000, 15);
			break;
		case 7:
			processEntry = load_elf(8500, 15);
			break;
		case 8:
			processEntry = load_elf(9000, 15);
			break;
		default:
			processEntry = load_elf(5000, 40);
			break;
	}
	//static fish_task_t *parent_task;
	//parent_task = (task_t *)current_ftask;
	uint32_t task_id = next_pid ;
	fish_task_t *fishTask = (fish_task_t *)kmalloc(sizeof(fish_task_t));
	
	// 将新进程写入到进程队列最后面
	fish_task_t *tmp_task = (fish_task_t *)task_queue;
	while(tmp_task -> next)
		tmp_task = tmp_task ->next;
	tmp_task -> next = fishTask;
	fishTask -> priority = priority;		// 进程优先级
	fishTask -> start_time = 0;			
	fishTask -> end_time = 0;	
	nfTasks ++;
	
	fishTask -> entry = processEntry;
	//fishTask -> tss.eip = processEntry;
	//fishTask -> tss.eflags = 0x202;	

	fishTask -> task_id = next_pid ++;
	fishTask -> next = NULL;		// 进程链表
	tss_entry = (tss_entry_t *)kmalloc(103);
	memset(tss_entry, 0x00, 103);
	tss_entry -> ss0 = 0x10;
	tss_entry -> esp0 = ld_tss_stack + 4092;
	tss_entry -> ss = 0x10;
	tss_entry -> esp = 0x7fc8;
	//if(taskNum!=0) 
	{
	tss_entry -> eip = processEntry;
	tss_entry -> eflags = 0x202;
	tss_entry -> cs = 0x08;
	tss_entry -> es = 0x10;
	tss_entry -> ds = 0x10;
	tss_entry-> fs = 0x10;
	tss_entry -> gs = 0x10;
	
	//tss_entry -> esp = 0x7fc8;
	//fishTask -> tss -> ss0 = 0x20;
	//fishTask -> tss -> esp0 = 0xFFFFFFFF;
	}
	tss_entry -> iopb_offset = 0;
	fishTask -> tss_selector = tssNum<<3; 
	fishTask -> tss = tss_entry;
	//printk("tss_entry 0x%x processEntry 0x%x cs 0x%x tssNum 0x%x\n", tss_entry,fishTask->tss->eip,fishTask->tss->cs,fishTask -> tss_selector);
	write_tss(tss_entry,tssNum);
	tssNum ++;
	current_ftask = task_queue->next;
	asm volatile("sti");
	//switch_context(fishTask);
	return fishTask -> task_id;
}


// 加载进程到内存中
void load_task()
{
	task_array[0] = load_elf(5000, 40);	// hello.bin
	task_array[1] = load_elf(5500, 15);	// --> hello2.bin hello, os kernel
	task_array[2] = load_elf(6000, 15);	// --> hello3.bin hello, kernel
	// 定义第一个进程
	fish_task_t *ftask1 = kmalloc_a(0x1000);
	ftask1 -> task_id = 1;
	ftask1 -> entry = task_array[0];
	ftask1 -> start_time = 0;
	ftask1 -> end_time = 0;
	ftask1 -> priority = PRIO_HIGH;
	current_ftask = task_queue = ftask1;
	// 定义第二个进程
	fish_task_t *ftask2 = kmalloc_a(0x1000);
	ftask2 -> task_id = 2;
	ftask2 -> entry = task_array[1];
	ftask2 -> start_time = 0;
	ftask2 -> end_time = 0;
	ftask2 -> priority = PRIO_LOW;
	task_queue -> next = ftask2;

	// 定义第三个进程
	fish_task_t *ftask3 = kmalloc_a(0x1000);
	ftask3 -> task_id = 3;
	ftask3 -> entry = task_array[2];
	ftask3 -> start_time = 0;
	ftask3 -> end_time = 0;
	ftask3 -> priority = PRIO_MED;
	task_queue -> next ->next = ftask3;
}

fish_task_t *parent_of_task(fish_task_t *src)
{
	fish_task_t *tmp_task = (fish_task_t*)task_queue;
	while(tmp_task != 0)
	{
		if(tmp_task->next == src)
		{
			return tmp_task;
		}else{
			tmp_task = tmp_task->next;
		}
	}
	return 0;
}

struct task_array
{
	struct task *process;
};

void swap_task_locations(fish_task_t *dest, fish_task_t *src)
{
	//找到父进程
	fish_task_t *parent_dest = parent_of_task(dest);
	fish_task_t *parent_src = parent_of_task(src);

	//进程的next进程
	fish_task_t *src_next, *dest_next;
	src_next = src->next;
	dest_next = dest->next;

	//dest进程不是第一个进程
	if(parent_dest != 0)
	{
		parent_dest->next = src;
		src->next = src == dest_next ? dest : dest_next;
	}else{ //dest是第一个进程
		task_queue = src;
		src->next = src == dest_next ? dest : dest_next;
	}

	//parent_src 不是第一个任务
	if(parent_src != 0)
	{
		dest->next = src_next;
	}else{ //parent_src是一个任务

		task_queue = dest;
		dest->next = src_next;
	}

	
}
void exec();
void switch_process(fish_task_t * process);

// 将进程队列进行排序,当priority的数值越大,代表优先级越高
// 排到进程队列的前面
void schedule()	
{
	asm volatile("cli");
	fish_task_t *prev_task;
	fish_task_t *cur_task;
	uint32_t a, i, j = 0;
	//exec();
	for(a = 0; a < 10; a++)
	{
		prev_task = (fish_task_t*) task_queue;
		cur_task = (fish_task_t*) task_queue -> next;
		for(j =0 ; j < 10; j ++)
		{
			if((cur_task -> priority) > (prev_task -> priority) )
			{

				swap_task_locations(prev_task, cur_task);
				cur_task = cur_task->next == 0 ? 0 : cur_task->next->next;	
			}else{				
				prev_task = prev_task -> next;
				cur_task = cur_task -> next;
			}
		
			if(cur_task == 0)
			{
				break;
			}
		}
	}
	asm volatile("sti");	
	// 将排序之后的进程队列的优先级打印出来
	prev_task = (fish_task_t*) task_queue;
	while(prev_task)
	{
		//printk("task_queue task priority %d\n",prev_task -> priority);		
		process_to_run[i] = (uint32_t)prev_task -> entry;
		//printk("schedule location is: 0x%08x \n", prev_task -> entry);
		//printk("process to run 0x%08x\n", process_to_run);
		prev_task = prev_task -> next;
		if(prev_task -> priority == PRIO_IDLE){
			break;		
		}
		i++;
	}
	execute();
}
void execute()
{
	if(current_ftask == NULL) return;
	current_ftask = current_ftask->next;
	if(current_ftask -> priority == PRIO_IDLE){
		current_ftask = current_ftask -> next;		
	}
	if(current_ftask == NULL) {
		// 当前进程队列执行结束
 while (1)
    {
        // 从键盘获取一个输入的字符
        char keyboard_input = get_char_from_keyboard();
        // 输出该字符
        if (keyboard_input == 0x8)
        {
  
	   // 退格键
            if (command_len == 0)
            {
                continue;
            }
            if (command_len > 0)
            { 
                --command_len;
                console_putc_color(keyboard_input, rc_black, rc_white);
                console_putc_color(' ', rc_black, rc_white);
                console_putc_color(keyboard_input, rc_black, rc_white);
            }
        }
        else if (keyboard_input == '\n')
        {
            // 换行键
	    console_putc_color(keyboard_input, rc_black, rc_red);
            command[command_len++] = '\0';
            command_len = 0;
            
            // 解析命令行，执行相应操作
            parse_command();
	    addShellIndent();
        }else
	{
		console_putc_color(keyboard_input, rc_black, rc_red);
		command[command_len++] = keyboard_input;
	}
    }	
	}//current_ftask = task_queue->next;
	switch_process(current_ftask);
}

void exit()
{
	asm volatile("sti");
	
	current_task -> priority = PRIO_DEAD;
	current_task -> time_to_run = 0;
	current_task -> ready_to_run = 0;

	task_t *task_prev = 0;
	task_t *task_r = (task_t*)ready_queue;
	for(; task_r -> next != 0; task_r = task_r -> next)
	{
		if(task_r -> next == current_task)
		{
			task_prev = task_r;
			break;
		}
	}

	// 队列中没有找到
	if(!task_prev && current_task != ready_queue)
		return;
	// 如果是第一个	
	if(current_task == ready_queue)
	{
		ready_queue = current_task -> next;
	}else{
		task_prev -> next = current_task -> next;
	}

	nTasks--;

	//kfree((void*)current_task);
	//kfree((void*)current_task -> originalStack);
	//kfree((void*)current_task -> page_directory);

	asm volatile("sti");
	switch_task();

}


int32_t start_task(uint32_t priority, uint32_t burst_time, void(*func)(), void *arg, char *task_Name)
{
	asm volatile("cli");
	
	uint32_t id = next_pid ++;
	uint32_t *stack;
	task_t *task = (task_t *)kmalloc(sizeof(task_t));
	task -> stack = kmalloc(0x1000) + 0x1000;
	task -> esp = task -> stack;
	task -> originalStack = task -> stack - 0x1000;
	
	task -> page_directory = 0;
	task -> priority = priority;
	task -> time_to_run = 0;
	task -> ready_to_run = TRUE;

	task -> burst_time = burst_time;
	task -> averaged_priority = priority + burst_time;
	
	nTasks ++;
	stack = (uint32_t*)task -> stack;
	// iret
	*--stack = 0x202;	// EFLAGS
	*--stack = 0x08;		// CS
	*--stack = (uint32_t)func;	// EIP -> 函数运行指针
	
	// pusha
	*--stack = 0;	 // EDI
	*--stack = 0;	// ESI
	*--stack = 0;	// EBP
	*--stack = 0;	// EDX
	*--stack = 0;	// ECX
	*--stack = 0;	// EAX	

	// data segment
	*--stack = 0x10;	// DS
	*--stack = 0x10;	// ES
	*--stack = 0x10;	// FS
	*--stack = 0x10;	// GS

	task -> id = id;
	task -> stack = (uint32_t)stack;
	task -> thread = func;
	task -> thread_flags = (uint32_t)arg;

	task -> next = 0;
	asm volatile("sti");
	return id;

}

int kill_task(uint32_t pid)
{
	task_t *task_prev = 0;
	task_t *task_r = (task_t *)ready_queue;
	for(; task_r -> next != 0; task_r = task_r -> next)
	{
		if(task_r -> next -> id == pid)
		{
			task_prev = task_r;
		}
	}
	if(! task_prev)
		return 1;

	task_r = task_prev -> next;
	task_prev -> next = task_r -> next;
	nTasks --;
	//kfree((void*)task_r);
	return 0;
}

void switch_to_user_mode()
{
	  asm volatile(" \
		cli; \
		mov $0x23, %ax; \
		mov %ax, %ds; \
		mov %ax, %es; \
		mov %ax, %fs; \
		mov %ax, %gs; \
		\
		\
		mov %esp, %eax; \
		pushl $0x23; \
		pushl %esp; \
		pushf; \
		pushl $0x1B; \
		push $1f; \
		iret; \
		1: \
		");
}


void tasking_test()
{
	char *arguements;
	if(current_task -> thread_flags)
	{
		arguements = (char*)current_task -> thread_flags;	
	}

	asm volatile("sti");
	
	// 如果 "tasking" 可以运行，则多进程成功
	if(!strcmp(arguements, "tasking"))
	{
		printk("\t Multitasking successed with task PID: %d\n", getpid());
	}else
	{
		printk("\t Multitasking failed.\n");
	}
	//退出
	exit();
}
