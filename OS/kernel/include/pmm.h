
#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

#include "common.h"

// 内核文件在内存中的起始和结束位置
// 在链接器脚本中要求链接器定义
extern uint8_t kern_start[];
extern uint8_t kern_end[];


//初始化内存分配
uint32_t kmalloc_int(uint32_t size, int align, uint32_t *phys);
//分配页对齐大小的内存
uint32_t kmalloc_a(uint32_t size);
//分配内存，返回物理内存地址
uint32_t kmalloc_p(uint32_t size, uint32_t *phys);

//分配内存，返回物理地址，页对齐
uint32_t kmalloc_ap(uint32_t size, uint32_t *phys);

//简单分配
uint32_t kmalloc(uint32_t size);

#endif 	// INCLUDE_PMM_H

