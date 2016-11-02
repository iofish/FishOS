#ifndef PAGING_H_
#define PAGING_H_

#include "common.h"
#include "idt.h"

typedef struct page
{
	uint32_t present	: 1; //内存中存在
	uint32_t rw		: 1; //1的时候读写，0只读
	uint32_t user		: 1; // 用户态
	uint32_t accessed	: 1; // 之前可以用
	uint32_t dirty		: 1; // 之前使用过
	uint32_t unused		: 7; // 保留
	uint32_t frame		:20; // 帧地址（右移12位）
}page_t;	// 32bit 4 Bytes

typedef struct page_table
{
	page_t pages[1024];	//1024个页表
}page_table_t; // 4KB
	
typedef struct page_directory
{
	page_table_t *tables[1024]; //页表的地址
	uint32_t tablesPhysical[1024]; //页表对应的物理地址
	uint32_t physicalAddr; // 物理地址表的物理地址
} page_directory_t; 	//8KB

//初始化页表
void init_paging();

//通过CR3来储存页表目录
void switch_page_directory(page_directory_t *new);
//获取页表，make字段为1，则需要创建页表
page_t *get_page(uint32_t address, int make, page_directory_t *dir);

// 页错误
void page_fault(pt_regs regs);
// 复制页目录
page_directory_t *clone_directory(page_directory_t *src);

#endif //PAGING_H_
