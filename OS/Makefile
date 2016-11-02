#!/bin/bash

C_SOURCES = $(shell find ./kernel -name "*.c")
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
S_SOURCES = $(shell find ./kernel -name "*.s")
S_OBJECTS = $(patsubst %.s, %.o, $(S_SOURCES))

LD_FILE = -s -Ttext 0x100000
IMG = fishos.img
ELF = -f elf -o
BIN = kernel.bin

GCC = gcc 
C_FLAGS = -c -m32 -nostdinc -fno-builtin -fno-stack-protector -I include
all:boot fat32bin   kernel fatmat 

.PHONY:boot
boot:
	nasm boot.asm -o boot.bin

.PHONY:fat32bin
fat32bin:
	nasm fat32.asm -o fat32.bin

.PHONY:kernel
kernel:
	cd ./kernel && make all
	
#location of first sector of root directory
.PHONY:xxdloader
xxdloader:
	xxd -s1851392 -l 512 $(IMG)

.PHONY:xxdboot
xxdboot:
	xxd -s0 -l 512 $(IMG)
.PHONY:xxdkernel
xxdkernel:
	xxd -s1851904 -l 512 $(IMG)

.PHONY:xxddboot
xxddboot:
	xxd -s1048576 -l 2048 $(IMG)
.PHONY:fatmat
fatmat:
	g++ -O2 -g -Wall -fmessage-length=0   -c -o fatmat.o fatmat.cpp
	g++ -o fatmat fatmat.o
	sh ./diskimage.sh
	./fatmat
.PHONY:bochs
bochs:
	bochs

.PHONY:qemu
qemu:
	qemu-system-i386 -hda fishos.img 
.PHONY:clean
clean:
	rm -rf *.bin *.o *~ fatmat
	cd ./kernel && make clean
