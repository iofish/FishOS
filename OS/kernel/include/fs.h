#ifndef FISH_OS_FS_H
#define FISH_OS_FS_H
#include "fat32.h"

	//FAT12/16/32公有
struct fat32_bpb{
	uint8_t jmp[3];
	char oem_ident[8];
	uint16_t bytes_per_sector;
	uint8_t sector_per_cluster;
	uint16_t reserved_sectors;
	uint8_t num_fats;
	uint16_t num_direntries;
	uint16_t small_total_sectors;
	uint8_t media_descr_type;	// 0xf8 硬盘
	uint16_t ignore;		// fat12/fat16使用
	uint16_t sect_per_track;
	uint16_t heads;
	uint32_t hidden_sectors;
	uint32_t total_sectors; 
	//FAT32 only
	uint32_t sectors_per_fat;
	uint16_t flags;
	uint8_t fat_major_version;
	uint8_t fat_minor_version;
	uint32_t root_cluster_num;
	uint16_t fsinfo_cluster_num;
	uint8_t reserved[12];	// 置0
	uint8_t drive_number;
	uint8_t reserved2;	// 保留
	uint8_t signature;	// 0x28 or 0x29
	uint32_t volumeid_serial;
	char volume_lable[11];	// space
} __attribute__((packed));

typedef struct fat32_bpb fat32_bpb_t;

#define FAT32_MAFIC 0x1234fedc

typedef struct fat32_partition{
	uint32_t magic;	//验证
	uint32_t fat_start_lba;	// LBA起始位置
	uint32_t end_lba;	//分区可以使用的最后位置
	uint32_t cluster_start_lab;
	uint32_t sectors_per_cluster;
	uint32_t root_dir_first_cluster;
	uint32_t cluster_size;	
	
}fat32_partition_t;

//fat32目录使用的时间格式
typedef struct fat32_time{
	uint16_t second : 5;
	uint16_t minute	: 6;
	uint16_t hour	: 5;
}__attribute__((packed)) fat32_time_t;

//fat32目录使用的日期格式 1970-1-1
typedef struct fat32_date{
	uint16_t day	: 5;
	uint16_t month	: 4;
	uint16_t year	: 7;
}__attribute__((packed)) fat32_date_t;

//fat32 目录
typedef struct fat32_direntry{
	char name[11];		// 8.3 文件名,如果小于8,空格补齐
	uint8_t attrib;
	uint8_t reserved;
	uint8_t create_10ths;
	
	fat32_time_t create_time;
	fat32_date_t create_date;
	
	fat32_date_t access_date;
	
	uint16_t high_cluster_bum;
	
	fat32_time_t mod_time;
	fat32_date_t mod_date;
	
	uint16_t low_cluster_num;
	uint32_t file_size;
}__attribute__((packed)) fat32_direntry_t;

//fat32目录映射
typedef struct fat32_lfn{
	uint8_t entry;
	uint16_t name_1[5];
	uint8_t attrib;			// 长目录时0xf
	uint8_t long_entry_type;	// 长目录时0
	uint8_t checksum;
	uint16_t name_2[6];
	char zero[2];			// 0
	uint16_t name_3[2];
}__attribute__((packed)) fat32_lfn_t; 

#endif //FISH_OS_FS_H
