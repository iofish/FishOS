
#include "gdt.h"
#include "string.h"

// 全局描述符表长度
#define GDT_LENGTH 30

// 全局描述符表定义
gdt_entry_t *gdt_entries;

// GDTR
gdt_ptr_t gdt_ptr;


// LDT 选择子
uint32_t INDEX_LDT_FIRST = 6;
// TSS
//tss_entry_t tss_entry;
// 全局描述符表构造函数，根据下标构造
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

// 声明内核栈地址
extern uint32_t stack;
extern void tss_flush();

// 初始化全局描述符表
void init_gdt()
{
	// 全局描述符表界限 e.g. 从 0 开始，所以总长要 - 1
	gdt_entries = (gdt_entry_t *)kmalloc(sizeof(gdt_entry_t)*8);
	gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_LENGTH) - 1;
	gdt_ptr.base = (uint32_t)gdt_entries;
	printk("gdt_ptr.base: 0x%x\n",gdt_ptr.base);
	// 采用 Intel 平坦模型
	gdt_set_gate(0, 0, 0, 0, 0);             	// 按照 Intel 文档要求，第一个描述符必须全 0
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); 	// 指令段
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); 	// 数据段	
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); 	// 数据段	
	//gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); 	// 用户数据段
	//gdt_set_gate(3, 0x1000000, 0x1000, 0x96, 0xCF); 	// system stack
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); 	// 用户模式数据段
	

	// 填充 GDT 中进程的 LDT 的描述符
	//PROCESS* p_proc	= proc_table;
	//uint16_t selector_ldt = INDEX_LDT_FIRST;  // 5
	//for(int i=0; i < MAX_TASK; i++){
	//gdt_set_gate(5, 0, 0xFFFFFFFF, 0x82, );
	//selector_ldt ++;
	//}

	//write_tss(5, 0x10, 0x0);
	// 加载全局描述符表地址到 GPTR 寄存器
	gdt_flush((uint32_t)&gdt_ptr);	// TSS 从6开始
	
	//write_tss(5);

}

// 全局描述符表构造函数，根据下标构造
// 参数分别是 数组下标、基地址、限长、访问标志，其它访问标志
/* 结构体定义如下：
typedef struct
{
	uint16_t limit_low;     // 段界限   15～0
	uint16_t base_low;      // 段基地址 15～0
	uint8_t  base_middle;   // 段基地址 23～16
	uint8_t  access;        // 段存在位、描述符特权级、描述符类型、描述符子类别
	uint8_t  granularity; 	// 其他标志、段界限 19～16
	uint8_t  base_high;     // 段基地址 31～24
} __attribute__((packed)) gdt_entry_t;
*/
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt_entries[num].base_low     = (base & 0xFFFF);
	gdt_entries[num].base_middle  = (base >> 16) & 0xFF;
	gdt_entries[num].base_high    = (base >> 24) & 0xFF;

	gdt_entries[num].limit_low    = (limit & 0xFFFF);
	gdt_entries[num].granularity  = (limit >> 16) & 0x0F;

	gdt_entries[num].granularity |= gran & 0xF0;
	gdt_entries[num].access       = access;
}

void write_tss(tss_entry_t *tss_entry,int32_t num)
{
	//tss_entry_t *tss_entry = (tss_entry_t *)kmalloc(sizeof(tss_entry_t));

	// 计算基地址和界限
	uint32_t base = (uint32_t) tss_entry;
	uint32_t limit = 103;
	
	// 在GDT中添加TSS 描述符
	gdt_set_gate(num, base, limit, 0x89, 0x00);	
	// P = 1 TYPE = 9 TSS, G = 0 D/B =1
	/*gdt_entries[num].limit_low 	= limit&0xFFFF;
	gdt_entries[num].base_low  	= 0x08;
	gdt_entries[num].base_middle    = 0x00;

	gdt_entries[num].access    = 0x0c;
	gdt_entries[num].granularity  = (limit>>16)&0x00FF;

	gdt_entries[num].base_high = (limit>>24)&0x0F;*/
	// TSS描述符初始为0
	//memset(&tss_entry, 0, sizeof(tss_entry));
	//tss_entry -> cs = 0x08;
	//tss_entry -> ds = 0x10;
	//tss_entry -> ss = 0x20;

// 写入tss
	//gdt_flush((uint32_t)&gdt_ptr);
	//tss_flush();

	//tss_entry.ss = 0x10;
	//tss_entry.eip = ;

	/*tss_entry.ss0 = ss0;	// 内核堆栈 0x10 内核数据段选择符
	tss_entry.esp0 = esp0;	// 栈顶指针 堆栈栈底低指针
	
	// TSS中包括cs, ss, ds, es, fs, gs,这些段在处理器切换到内核模式下也需要加载
	// 因为 0x08 和 0x10 已经使用,所以使用 0x0b 和 0x13, 在内核从内核态切换到用户态
	// 时候也可以使用 ring 0 -> ring 3
	tss_entry.cs = 0x0b;
	tss_entry.ss = tss_entry.ds = tss_entry.es =tss_entry.fs = tss_entry.gs = 0x13;
*/
}


