[global switch_task_to]
; 相当于 switch_task_to 函数
; void switch_task_to(uint32_t *current, uint32_t *next)
; 具体的进程切换操作，寄存器的保存与恢复
switch_task_to:
        mov eax, [esp+4]	;保存 current地址

        mov [eax + 0],  gs	
        mov [eax + 4],  fs
        mov [eax + 8],  es
        mov [eax + 12], ds
        mov [eax + 16], eax
	mov [eax + 20], ecx
	mov [eax + 24], edx
	mov [eax + 28], ebx
				;mov [eax + 32], nop
	mov [eax + 36], ebp
	mov [eax + 40], esi
	mov [eax + 44], edi
	call fun1
	mov [eax + 48],ebx	; eip
	mov [eax + 52], cs
				;mov [eax + 56], eflags
        pushf
        pop ecx
        mov [eax + 56], ecx

        mov eax, [esp+8]	;保存 next 地址

        mov [eax + 0],  gs	
        mov [eax + 4],  fs
        mov [eax + 8],  es
        mov [eax + 12], ds
        mov [eax + 16], eax
	mov [eax + 20], ecx
	mov [eax + 24], edx
	mov [eax + 28], ebx
				;mov [eax + 32], nop
	mov [eax + 36], ebp
	mov [eax + 40], esi
	mov [eax + 44], edi
	;mov [eax + 48], eip
	mov [eax + 52], cs
				;mov [eax + 56], eflags
        push eax
        popf
 	
        ret
fun1:
	pop ebx
	jmp ebx
