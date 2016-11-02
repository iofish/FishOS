#ifndef INCLUDE_ELF_H_
#define INCLUDE_ELF_H_

#include "types.h"

//程序头类型定义
#define PT_NULL	0	// NULL
#define PT_LOAD	1	// 可加载的段
#define PT_DYNAMIC 2	// 动态段
#define PT_INTERP 3	// 解释器的位置和大小
#define	PT_NOTE 4	// 记录段
#define PT_SHLIB 5	// 保留
#define PT_PHDR 6	// 程序头的位置和大小
#define PT_HIPROC 0x7fffffff
#define PT_LOPROC 0x70000000

// ELF 程序头
struct elfhdr
{
	uint8_t ident[16];	// elf 头标志
	uint16_t type;		// elf 头文件的类型, 2是可执行, 3是库文件
	uint16_t machine;	// 结构 7 表示 x86
	uint32_t version;	// 1表示可用
	uint32_t entry;		// 入口指针, EIP指针指向的位置
	uint32_t phoff;		// 程序头结构的偏移
	uint32_t shoff;		// 节结构的偏移
	uint32_t flag;		// 0 表示 x86
	uint16_t ehsize;	// elf头大小
	uint16_t phentsize;	// 程序头的大小
	uint16_t phnum;		// 程序头的数量
	uint16_t shentsize;	// 节头的大小
	uint16_t shnum;		// 节头的数量
	uint16_t shstrndx;	// 包含节名称的字符串是第几个节
}__attribute__((packed));

typedef struct elfhdr elfHeader;

// progarm header
struct prgmhdr
{
	uint32_t type;		// PT_LOAD 1
	uint32_t offset;	// 段在在文件中的偏移
	uint32_t vaddr;		// 段在内存中的虚拟地址
	uint32_t paddr;		// 段在内存中的物理地址
	uint32_t fileSize;	// 段在文件中的长度
	uint32_t memSize;	// 段在内存中的长度	
	uint32_t flags;		// 段的标记位
	uint32_t alignment;	// 段对齐 0X1000H
}__attribute__((packed));

typedef struct prgmhdr programHeader;

// 执行给定地址的二进制文件
void execute_binary(uint32_t* addr);

// 加载elf文件， sect_nr为读取的扇区位置， sect_size为读取大小
uint32_t load_elf(uint32_t sect_nr , uint32_t sect_size);

// 分配elf文件
uint32_t reloc_elf(elfHeader *header, uint32_t offset);

void elf_test();

elfHeader* parse_elf(elfHeader* addr);

#endif 	// INCLUDE_ELF_H_
