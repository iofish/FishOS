[global switch_to]
[global exec]
[global switch_process]
[global switch_idle]
; 具体的线程切换操作，重点在于寄存器的保存与恢复
switch_to:
        mov eax, [esp+4]

        mov [eax+0],  esp
        mov [eax+4],  ebp
        mov [eax+8],  ebx
        mov [eax+12], esi
        mov [eax+16], edi
        pushf
        pop ecx
        mov [eax+20], ecx

        mov eax, [esp+8]

        mov esp, [eax+0]
        mov ebp, [eax+4]
        mov ebx, [eax+8]
        mov esi, [eax+12]
        mov edi, [eax+16]
        mov eax, [eax+20]
        push eax
        popf
 	
        ret

exec:
	push eax
	mov ax,0x28
	ltr ax
	pop eax
	ret
	;mov eax,0x10
	;push eax
	;mov eax,esp
	;push eax
	;mov eax,0x202
	;push eax
	;mov eax,0x08
	;push eax
	;mov eax,0x200020
	;push eax
	;iret
switch_process:
	;push 0x25000c
	;jmp 0x25000c
	mov eax,[esp+4]
	;call far [eax+4] ; call 和 iret仅仅是一个链表形式的进程跳转,不符合
	;当前进程跳转的方式
	jmp far [eax + 4];  需要将跳转方式改成jmp,同时加上中断进行跳转
	;jmp 0x38:0
	;mov eax,[0x135e20+0x40]
	;ret

switch_idle:
	;call 0x28:0

flush:
	;ret

