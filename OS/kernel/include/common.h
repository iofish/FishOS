
#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include "types.h"

// 端口写一个字节
void outb(uint16_t port, uint8_t value);

// 端口读一个字节
uint8_t inb(uint16_t port);

// 端口读一个字
uint16_t inw(uint16_t port);
//2016-05-14 09:53:26
// 端口读取n个数据，存在buf中
void port_read(uint16_t port, void* buf, int n);

// 端口写入n个数据，buf中的数据
void prot_write(uint16_t port, void* buf, int n);

#endif // INCLUDE_COMMON_H_
