//2016-05-13 21:53:56
// FAT32.h 使用fat32文件系统处理文件

#ifndef FISHOS_FAT32_H
#define FISHOS_FAT32_H

#include "common.h"

#define MAKE_DEVICE_REG(lba, drv, lba_highest) (((lba)<<6) | ((drv)<<4) | (lba_highest & 0xF) | 0xA0) // 第6位是打开LBA模式， 第4位是选择主盘或从盘，3 - 0位， l = 0 时，对磁盘使用CHS模式， l = 1时，对磁盘使用LBA模式，并且是LBA的24 -27 位

void init_fat32();

struct fat32_cmd{
	uint8_t features;	//
	uint8_t count;
	uint8_t lba_low;
	uint8_t lba_mid;
	uint8_t lba_high;
	uint8_t device;
	uint8_t command;
};

void hd_read(uint32_t sect_nr, uint32_t sects_to_read, uint8_t *buf);
void hd_write(uint32_t sect_nr, uint32_t sects_to_read, uint8_t *buf);

#endif //FISHOS_FAT32_H
