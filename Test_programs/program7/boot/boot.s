MBOOT_HEADER_MAGIC 	equ 	0x1BADB002 	
MBOOT_PAGE_ALIGN 	equ 	1 << 0    	
MBOOT_MEM_INFO 		equ 	1 << 1    	

MBOOT_HEADER_FLAGS 	equ 	MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM 		equ 	- (MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)
[BITS 32]  	; 所有代码以 32-bit 的方式编译

section .text 	; 代码段从这里开始

; 在代码段的起始位置设置符合 Multiboot 规范的标记

dd MBOOT_HEADER_MAGIC 	; GRUB 会通过这个魔数判断该映像是否支持
dd MBOOT_HEADER_FLAGS   ; GRUB 的一些加载时选项，其详细注释在定义处
dd MBOOT_CHECKSUM       ; 检测数值，其含义在定义处

[GLOBAL start] 		; 内核代码入口，此处提供该声明给 ld 链接器
[EXTERN kern_entry] 	; 声明内核 C 代码的入口函数

start:
	cli  			 ; 此时还没有设置好保护模式的中断处理，要关闭中断
				 ; 所以必须关闭中断
	mov esp, STACK_TOP  	 ; 设置内核栈地址
	mov ebp, 0 		 ; 帧指针修改为 0
	and esp, 0FFFFFFF0H	 ; 栈地址按照16字节对齐
	call kern_entry		 ; 调用内核入口函数
stop:
	hlt 			 ; 停机指令，什么也不做，可以降低 CPU 功耗
	jmp stop 		 ; 到这里结束，关机什么的后面再说

;-----------------------------------------------------------------------------

section .bss 			 ; 未初始化的数据段从这里开始
stack:
	resb 32768 	 	 ; 这里作为内核栈

STACK_TOP equ $-stack-1 	 ; 内核栈顶，$ 符指代是当前地址

;-----------------------------------------------------------------------------
