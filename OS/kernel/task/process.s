[GLOBAL read_eip]
read_eip:
    pop eax                     ; 返回地址
    jmp eax                     ; 从栈中pop出来的返回值，直接跳转就可以了，不能使用ret 

[GLOBAL copy_page_physical]
copy_page_physical:
    push ebx              ; 根据 __cdecl（右到左入栈）, 保存ebx的值
    pushf                 ; 讲寄存器的值入栈
                          ; 
    cli                   ; 关中断，不能被中断影响
                          ; 在分页关闭之前，保存寄存器的值!
    mov ebx, [esp+12]     ; 源地址Source address
    mov ecx, [esp+16]     ; 目标地址Destination address
  			  ; 这里为什么是 +12 +16？为何不是+4 +8？
    mov edx, cr0          ; 获取CR0寄存器的值
    and edx, 0x7fffffff   ; 将第31位置0
    mov cr0, edx          ; 关闭分页
  
    mov edx, 1024         ; 1024*4bytes = 4096 bytes
  
.loop:
    mov eax, [ebx]        ; 得到ebx 源地址的Word 4 个字节
    mov [ecx], eax        ; 存在目标地址中
    add ebx, 4            ; Source address += sizeof(word)
    add ecx, 4            ; Dest address += sizeof(word)
    dec edx               ; edx - 1
    jnz .loop             
  
    mov edx, cr0          ; 
    or  edx, 0x80000000   ; 将31位置 1
    mov cr0, edx          ; 开分页
  
    popf                  ; 出栈之前保存的值
    pop ebx               ; pop 之前保存的ebx
    ret
