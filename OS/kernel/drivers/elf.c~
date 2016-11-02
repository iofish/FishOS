//2016-05-29 11:02:58

#include "elf.h"
#include "fat32.h"
#include "string.h"

uint32_t Sector_Size = 512;

void elf_test()
{
	uint8_t* hdbuf = kmalloc_a(40 * Sector_Size); 	//hello world 程序为40个扇区大小
	uint32_t offset = hdbuf;
	hd_read(5000, 40, hdbuf);			// 在镜像文件中找到该程序
	elfHeader *elfTest;				
	elfTest = parse_elf(hdbuf);			// elfTest 指向elf头文件内存中首地址
	printk("0x%08x\n", offset);			// 解析elf文件
	reloc_elf(elfTest, offset);
	execute_binary(elfTest -> entry);
	//asm volatile("jmp %0" : : "a" (elfTest -> entry));

}




elfHeader* parse_elf(elfHeader* elfheader)		//解析elf头文件 0x34字节
{
	//printk("[parse_elf] ELF header #addr : %08x type:%08x, machine:%08x, version:%08x, entry:%08x, phoff:%d shoff:%d flag:%d, ehsize:%d, phentsize:%08x, phnum:%08x, shentsize:%08x, shnum: %08x, shstrndx:%08x \n",  elfheader, elfheader->type, elfheader->machine, elfheader->version, elfheader->entry, elfheader->phoff, elfheader->shoff, elfheader->flag, elfheader->ehsize, elfheader->phentsize, elfheader->phnum, elfheader -> shentsize, elfheader -> shnum, elfheader -> shstrndx);

	return elfheader;
}

void execute_binary(uint32_t* addr)
{
	//printk("addr values : 0x%08x \n", addr);
	asm volatile("jmp %0" : : "a" (addr));
}

// 加载elf
uint32_t load_elf(uint32_t sect_nr ,uint32_t sect_size)
{

	uint32_t* hdbuf = kmalloc_a(sect_size * Sector_Size);
	uint32_t offset = hdbuf;	//hdbuf的物理地址
	hd_read(sect_nr, sect_size, hdbuf);
	elfHeader *elfFile;
	elfFile = parse_elf(hdbuf);
	//printk(" pre elfFile->entry :0x%x\n",elfFile->entry);
	reloc_elf(elfFile, offset);
	//printk(" after elfFile->entry :0x%x\n",elfFile->entry);
	return elfFile -> entry;
	//execute_binary(elfFile -> entry);
}

// offset是地址空间
uint32_t reloc_elf(elfHeader *header, uint32_t offset)
{
	uint32_t i;
	programHeader *pHeader;
	//printk("%d program headers found, phentsize %d.\n", header -> phnum, header -> phentsize);
	for(i = 0; i < header -> phnum; i++)
	{
		pHeader = (programHeader *)((uint32_t) header + header -> phoff + i * header -> phentsize);
		//printk("[reloc_elf] Program header #%d Type:LOAD Offset:%08x Vaddr:%08x, Paddr:%08x, Size:%08x, MEM:%08x, Align:%08x!\n", i, pHeader->offset, pHeader->vaddr, pHeader->paddr, pHeader->fileSize, pHeader->memSize, pHeader->alignment);
		
		//将程序的数据拷贝到物理内存中
		//printk("pHeader -> paddr = 0x%08x, pHeader -> offset = 0x%08x\n",pHeader -> paddr, pHeader -> offset );
		memcpy((uint8_t *)pHeader -> paddr, (uint8_t *) (pHeader -> offset + offset), pHeader -> fileSize);
		if(pHeader -> fileSize < pHeader -> memSize)
		{
			memset((char *)pHeader -> paddr + pHeader -> fileSize, 0, pHeader -> memSize - pHeader -> fileSize);
		}
	}
	return 0;
}
