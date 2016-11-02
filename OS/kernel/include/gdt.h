

#ifndef INCLUDE_GDT_H_
#define INCLUDE_GDT_H_


#include "types.h"

typedef char ldsymbol[];
// 全局描述符类型
typedef
struct gdt_entry_t {
	uint16_t limit_low;     // 段界限   15～0
	uint16_t base_low;      // 段基地址 15～0
	uint8_t  base_middle;   // 段基地址 23～16
	uint8_t  access;        // 段存在位、描述符特权级、描述符类型、描述符子类别
	uint8_t  granularity; 	// 其他标志、段界限 19～16
	uint8_t  base_high;     // 段基地址 31～24
} __attribute__((packed)) gdt_entry_t;

// GDTR
typedef
struct gdt_ptr_t {
	uint16_t limit; 	// 全局描述符表限长
	uint32_t base; 		// 全局描述符表 32位 基地址
} __attribute__((packed)) gdt_ptr_t;


// TSS 任务状态
struct tss_entry_struct{
    uint16_t link;
    uint16_t unused1;
    uint32_t esp0;
    uint16_t ss0;
    uint16_t unused2;
    uint32_t esp1;
    uint16_t ss1;
    uint16_t unused3;
    uint32_t esp2;
    uint16_t ss2;
    uint16_t unused4;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint16_t es;
    uint16_t unused5;
    uint16_t cs;
    uint16_t unused6;
    uint16_t ss;
    uint16_t unused7;
    uint16_t ds;
    uint16_t unused8;
    uint16_t fs;
    uint16_t unused9;
    uint16_t gs;
    uint16_t unused10;
    uint16_t ldtr;
    uint16_t unused11;
    uint16_t unused12;
    uint16_t iopb_offset;
}__attribute__((packed));

typedef struct tss_entry_struct tss_entry_t;

//TSS设置内核栈
void set_kernel_stack(uint32_t stack);
// 初始化全局描述符表
void init_gdt();

// 写入tss
void write_tss();
// GDT 加载到 GDTR 的函数[汇编实现]
extern void gdt_flush(uint32_t);

#endif 	// INCLUDE_GDT_H_
