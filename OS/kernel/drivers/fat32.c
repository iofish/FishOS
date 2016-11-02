#include "fat32.h"
#include "idt.h"
#include "debug.h"

static int flag = 0;
uint8_t hdbuf[1024];

// 硬盘中断处理函数
static void hd_callback(pt_regs *regs)
{
	flag = 1;
	//printk("hd flag: %d\n", flag);
}

static void print_identify_info(uint16_t* hdinfo)
{
	int i, k;
	char s[64];

	struct iden_info_ascii {
		int idx;
		int len;
		char * desc;
	} iinfo[] = {{10, 20, "HD SN"}, /* Serial number in ASCII */
		     {27, 40, "HD Model"} /* Model number in ASCII */ };

	for (k = 0; k < sizeof(iinfo)/sizeof(iinfo[0]); k++) {
		char * p = (char*)&hdinfo[iinfo[k].idx];
		for (i = 0; i < iinfo[k].len/2; i++) {
			s[i*2+1] = *p++;
			s[i*2] = *p++;
		}
		s[i*2] = 0;
		printk("%s: %s\n", iinfo[k].desc, s);
	}

	int capabilities = hdinfo[49];
	printk("LBA supported: %s\n",
	       (capabilities & 0x0200) ? "Yes" : "No");

	int cmd_set_supported = hdinfo[83];
	printk("LBA48 supported: %s\n",
	       (cmd_set_supported & 0x0400) ? "Yes" : "No");

	int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];
	printk("HD size: %dMB\n", sectors * 512 / 1000000);
	//flag = 0;
}

void init_fat32()
{
	//注册硬盘中断处理函数
	register_interrupt_handler(IRQ14, hd_callback);
	//uint8_t *pNrDrives = (uint8_t*) 0x475;	//从BIOS总得到驱动的数量
	//printk("NrDrives : %d\n", *pNrDrives);
	//
	struct fat32_cmd cmd;
	cmd.device = MAKE_DEVICE_REG(0, 0, 0);	// 1F0h
	cmd.command = 0xec;			// 1F1h ATA_IDENTIFY识别用的， ATA_READ -> 0x20, ATA_WRITE -> 0x30
	outb(0x3f6, 0);				// Data
	outb(0x1f1, 0);				// Features
	outb(0x1f2, 0);				// Sector Count
	outb(0x1f3, 0);				// Sector Number | LBA Low 0 - 7 bits
	outb(0x1f4, 0);				// Cylinder Low | LBA Mid 8 -15 bits
	outb(0x1f5, 0);				// Cylinder High | LBA High 16 -23 bits
	outb(0x1f6, cmd.device);		// Drive
	outb(0x1f7, cmd.command);		// Status

	

	//检查硬盘
	port_read(0x1f0 , hdbuf, 512);
	print_identify_info((uint16_t*)hdbuf);	

	while(!flag);
		flag = 0;
	///asm volatile("sti");
	// 测试hd_read(),hd_write();
	//uint8_t *hdbuf = "hello geek!";
	//uint8_t *read_buf;	
	//hd_write(5000, 1, hdbuf);
	//hd_read(5000, 1, hdbuf);
}

//sect_nr:要读取的起始扇区号；
//sects_to_read:要读取的扇区数
//buf:缓存
void hd_read(uint32_t sect_nr, uint32_t sects_to_read, uint8_t *buf)
{
     	
	struct fat32_cmd cmd;
	cmd.features = 0;
	cmd.count = sects_to_read;
	cmd.lba_low = sect_nr & 0xff;
	cmd.lba_mid = (sect_nr >> 8) & 0xff;
	cmd.lba_high = (sect_nr >> 16) & 0xff;
	cmd.device = MAKE_DEVICE_REG(1, 0, (sect_nr >> 24) & 0xf);// LBA模式， 0 - 3 位是LBA的24 - 27位
	cmd.command = 0x20;	// 读数据
	outb(0x3f6, 0);
	outb(0x1f1, cmd.features);
	outb(0x1f2, cmd.count);
	outb(0x1f3, cmd.lba_low);
	outb(0x1f4, cmd.lba_mid);
	outb(0x1f5, cmd.lba_high);
	outb(0x1f6, cmd.device);
	outb(0x1f7, cmd.command);
	//asm volatile("cli");//读取的时候偶尔读取正确,加上一个中断
	//printk("");
	//while(!flag);
	//flag = 0;
	//printk("port start read buf!\n");
	port_read(0x1f0, buf, sects_to_read*512);
	//printk("port read finished! \n");
	//uint8_t tmp[512];
	//memcpy((uint8_t *)tmp, (uint8_t)*buf, 512);
	//printk("Read Sector values:\n%s\n", tmp);
	//while(!flag){
	//	flag = 0;
	
}


 //sect_nr:要写入的起始扇区号；
 //sects_to_write:要写入的扇区数
 //buf:要写的数据
void hd_write(uint32_t sect_nr, uint32_t sects_to_write, uint8_t *buf)
{
     //asm volatile("sti");    
     struct fat32_cmd cmd;
     cmd.features = 0;
     cmd.count    = sects_to_write;                                 //要写入的扇区数
     cmd.lba_low  = sect_nr & 0xff;
     cmd.lba_mid  = (sect_nr >> 8) & 0xff;
     cmd.lba_high = (sect_nr >> 16) & 0xff;
     cmd.device   = MAKE_DEVICE_REG(1, 0, (sect_nr >> 24) & 0xF); 
     cmd.command  = 0x30;                                            //尝试写扇区

     outb(0x3f6, 0);
     outb(0x1f1, cmd.features);
     outb(0x1f2, cmd.count);
     outb(0x1f3, cmd.lba_low);
     outb(0x1f4, cmd.lba_mid);
     outb(0x1f5, cmd.lba_high);
     outb(0x1f6, cmd.device);
     outb(0x1f7, cmd.command);  
     port_write(0x1f0, buf, sects_to_write*512);
     //uint8_t write_tmp[512];
     //memcpy((uint8_t*)write_tmp, (uint8_t)*buf, 512);
     //printk("write values : %s \n", buf);
     //while(!flag)
    // flag = 0;
	asm volatile("sti");//读取的时候偶尔读取正确,加上一个中断

}


