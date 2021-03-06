[GLOBAL gdt_flush]

gdt_flush:
	mov eax, [esp+4]  ; 参数存入 eax 寄存器
	lgdt [eax]        ; 加载到 GDTR [修改原先GRUB设置]

	mov ax, 0x10      ; 加载我们的数据段描述符
	mov ds, ax        ; 更新所有可以更新的段寄存器
	mov es, ax
	mov fs, ax
	mov gs, ax
	;mov ax,	0x18
	mov ss, ax
	jmp 0x08:.flush   ; 远跳转，0x08是我们的代码段描述符
			  ; 远跳目的是清空流水线并串行化处理器
.flush:
	ret



[GLOBAL tss_flush]    
tss_flush:
    push ax
    mov ax, 	0x28	; 第5个选择子, 每一个9字节大小 RPL 设置为0      	
    ;ltr ax            	; 将0x28写入到任务状态寄存器中
    ;jmp ax
    pop ax    
    ret
