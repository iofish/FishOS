
#include "common.h"
#include "debug.h"
#include "pmm.h"
#include "paging.h"

//内存起始位置
uint32_t placement_address = (uint32_t)&kern_end;
//内存使用
uint32_t phys_mem_usage;

uint32_t kmalloc_int(uint32_t size, int align, uint32_t *phys)
{
	if(align == 1 && (placement_address & 0xffff000))
	{
		//对齐起始地址
		placement_address &= 0xfffff000;
		placement_address += 0x1000;		
	}
	if(phys)
	{
		*phys = placement_address;
	}
	uint32_t tmp = placement_address;
	placement_address += size;
	phys_mem_usage += size;
	//printk("current memory location is: 0x%08x\n", tmp);
	return tmp;
}

uint32_t kmalloc_a(uint32_t size)
{
    return kmalloc_int(size, 1, 0);
}

uint32_t kmalloc_p(uint32_t size, uint32_t *phys)
{
    return kmalloc_int(size, 0, phys);
}

uint32_t kmalloc_ap(uint32_t size, uint32_t *phys)
{
    return kmalloc_int(size, 1, phys);
}

uint32_t kmalloc(uint32_t size)
{
    return kmalloc_int(size, 1, 0);
}


