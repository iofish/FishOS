// 设置内存分页
#include "paging.h"
#include "pmm.h"
#include "debug.h"

//内核页目录
page_directory_t *kernel_directory = 0;
//当前页目录
page_directory_t *current_directory = 0;

//帧的位图 判断 使用或者空

uint32_t *frames;
uint32_t nframes;
//pmm.c中定义的，使用内存的值
extern uint32_t placement_address;

// 定义bit的值，来判断帧是否存在
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

//设置frames 的bit值
static void set_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;  //4k大小
	uint32_t idx = INDEX_FROM_BIT(frame);  // 0
	uint32_t off = OFFSET_FROM_BIT(frame); // 16
	frames[idx] |= (0x1 << off);		// 设置标记  16位置1
}

static void clear_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);	// 清除标记
}


//测试是否是一个帧
static uint32_t test_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x1 << off)); //测试标记
}

// Static function to find the first free frame.
static uint32_t first_frame()
{
    uint32_t i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] != 0xFFFFFFFF) //啥也没有
        {
            // 32字节循环查询
            for (j = 0; j < 32; j++)
            {
                uint32_t toTest = 0x1 << j;
                if ( !(frames[i]&toTest) )
                {
                    return i*4*8+j;
                }
            }
        }
    }
}

// 分配一个页帧
void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
    if (page->frame != 0)
    {
        return;
    }
    else
    {
        uint32_t idx = first_frame();
        if (idx == (uint32_t)-1)
        {
            // 空空
        }
        set_frame(idx*0x1000);
        page->present = 1;
        page->rw = (is_writeable)?1:0;
        page->user = (is_kernel)?0:1;
        page->frame = idx;
    }
}

// 释放掉帧
void free_frame(page_t *page)
{
    uint32_t frame;
    if (!(frame=page->frame))
    {
        return;
    }
    else
    {
        clear_frame(frame);
        page->frame = 0x0;
    }
}

void init_paging()
{
    // 设置一个最大值 16MB
    uint32_t mem_end_page = 0x1000000;
    nframes = mem_end_page / 0x1000;	//所有的帧数
    //printk("nframes = 0x%x \n", nframes);
    frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));//设置内存的所有帧的地址  0x00118000
    memset(frames, 0, INDEX_FROM_BIT(nframes));	//清0

    //生成页目录
    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));// 8K大小 0x00118000
    current_directory = kernel_directory;

//将使用的内存地址从0开始进行分页处理
    int i = 0;
    //while (i < placement_address)
	while(i < 0x1000000)    
	{
        alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
        i += 0x1000;//分配4K大小的帧
	//从0x00118000 开始分配页帧，每帧占据4个字节 一共有4K个帧，对应着就是4个二级目录 × 1024 × 4096 内存大小，
    }
    //printk("page alloc successed!\n");
    //页错误中断处理注册
    register_interrupt_handler(14, page_fault);

    // 现在可以开启分页了
    //printk("kernel_directory location : 0x%08x \n", kernel_directory);
    switch_page_directory(kernel_directory);//0x00119000
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // 开启分页!  cr0 0x80000011
    asm volatile("mov %0, %%cr0":: "r"(cr0));
    //printk("switch page directory successed!\n");
}

page_t *get_page(uint32_t address, int make, page_directory_t *dir)
{
    //将地址变成索引
    address /= 0x1000;
    // address页表的地址
    uint32_t table_idx = address / 1024; //每个页表存放1K页帧
    if (dir->tables[table_idx]) //页表存在
    {
	//printk("%d\n",(address%1024));  
        return &dir->tables[table_idx]->pages[address%1024];// 这里暂时只分配了4096个页帧，一个页目录
    }
    else if(make)
    {
        uint32_t tmp; // 新建了四个tmp的值，也就是4个页目录
        dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);  //table_idx = 0
	//分配页表在内存中的地址，页大小4K，目录1K 分配地址：0x0011b000 物理地址在  tmp = 0x0011b000 ，page_table_t 为4K大小
	//printk("dir->tables[table_idx] = 0x%08x\n", dir->tables[table_idx]);
        dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
/*
 *  这里的数据说明  placement_address(当前大小) 1 表
 *  16MB的数据 -> 4 tables(页表) -> 4 * 1024 pages(页)
 *  地址对应着就是内存中从 0x00118000 开始到 0x0011b000 之间的每个字节的值
 *  16KB字节对应着 16MB的数据
 *
*/
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else
    {
        return 0;
    }
}


void page_fault(pt_regs regs)
{
	uint32_t faulting_address;//CR2寄存器中存在值
	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
	printk("页错误发生: 0x%08x", faulting_address);
   
}


static page_table_t *clone_table(page_table_t *src, uint32_t *physAddr)
{
	//新建一个页表，页对齐
	page_table_t *table = (page_table_t *)kmalloc_ap(sizeof(page_table_t), physAddr);
	// 新建的页表为空
	memset(table, 0 , sizeof(page_table_t));
	// 每个页
	int i;
	for(i = 0; i < 1024; i ++)
	{
		if(src -> pages[i].frame)// 对应着帧
		{
			alloc_frame(&table -> pages[1], 0, 0);
			// 传值
			if (src->pages[i].present) table->pages[i].present = 1;
			if (src->pages[i].rw) table->pages[i].rw = 1;
			if (src->pages[i].user) table->pages[i].user = 1;
			if (src->pages[i].accessed) table->pages[i].accessed = 1;
			if (src->pages[i].dirty) table->pages[i].dirty = 1;			
			copy_page_physical(src->pages[i].frame*0x1000, table->pages[i].frame*0x1000); // 每次拷贝4KB process.s中
		}	
	}
	return table;
}


page_directory_t *clone_directory(page_directory_t *src)
{
	uint32_t phys;
	// 新建一个页目录，获取其物理地址
	page_directory_t *dir = (page_directory_t *)kmalloc_ap(sizeof(page_directory_t) , &phys);
	// 页目录为空
	memset(dir, 0, sizeof(page_directory_t));
	// page_directory_t 结构体偏移值
	uint32_t offset = (uint32_t)dir -> tablesPhysical - (uint32_t)dir; // 4KB
	//printk("the offset values 0x%08x\n", offset);// 0x1000 4KB
	dir -> physicalAddr = phys + offset; // 指向page_directory_t 中的
					     //tablesPhysical变量 的地址
	// 开始复制页表了
	int i;
	for(int i; i < 1024; i++)	
	{
		if(!src -> tables[i])
			continue;
		if(kernel_directory -> tables[i] == src -> tables[i])
		{
			//内核目录。。。  不变就行
			dir -> tables[i] = src -> tables[i];
			dir -> tablesPhysical[i] = src -> tablesPhysical[i];
		}else
		{
			//复制
			uint32_t phys;
			dir -> tables[i] = clone_table(src -> tables[i], &phys);
			dir -> tablesPhysical[i] = phys | 0x07;
		}
	}	
	return dir;
}
