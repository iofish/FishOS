;*************************************************************************
; fishos kernel fat32.asm for the 2048 sector
;*************************************************************************
[BITS 16]
org 0100h

jmp     START
     

     OEM_ID                db           "FishOS  "      ; OEM String, 必须 8 个字节
     BytesPerSector        dw           0x0200          ; 每扇区字节数
     SectorsPerCluster     db           0x01            ; 每簇多少扇区
     ReservedSectors       dw           0x0020          ; 从卷的第一个扇区开始的保留扇区数目；
     TotalFATs             db           0x02            ; 卷上FAT数据结构的数目，该值通常应为2
     MaxRootEntries        dw           0x0000          ; 根目录文件数最大值
     NumberOfSectors       dw           0x0000          ; 该卷上的扇区总数
     MediaDescriptor       db           0xF8            ; 介质类型
     SectorsPerFAT         dw           0x0000          ; 该字段标识一个FAT结构占有的扇区数
     SectorsPerTrack       dw           0x0020          ; 每磁道扇区数
     SectorsPerHead        dw           0x0040          ; 磁头数(面数)
     HiddenSectors         dd           0x00000000      ; 包含该FAT卷的分区之前的隐藏扇区数
     TotalSectors          dd           0x001085b0              ; 该字段包含该卷上的所有扇区数目        
     BigSectorsPerFAT      dd           0x00000300
     Flags                 dw           0x0000
     FSVersion             dw           0x0000
     RootDirectoryStart    dd           0x00000002
     FSInfoSector          dw           0x0001
     BackupBootSector      dw           0x0006

     TIMES 12 DB 0                                      ; 保留字段，格式化程序应将该字段清0
     DriveNumber           db           0x80            ; 用于INT 0x13的驱动器号
     Signature             db           0x29            ; 扩展引导标志
     VolumeID              dd           0x5b2e2fa0      ; 卷序列号
     VolumeLabel           db           "FishOS  FAT"; 卷标, 必须 11 个字节
     SystemID              db           "FAT32   "      ; 文件系统类型
     ;2016.4.2
     %include "pm.inc"
     ;GDT

     LABEL_GDT:          Descriptor 0,            0, 0              ; 空描述符
     LABEL_DESC_FLAT_C:  Descriptor 0,      0fffffh, DA_CR|DA_32|DA_LIMIT_4K ;0-4G
     LABEL_DESC_FLAT_RW: Descriptor 0,      0fffffh, DA_DRW|DA_32|DA_LIMIT_4K;0-4G
     LABEL_DESC_VIDEO:   Descriptor 0B8000h, 0ffffh, DA_DRW|DA_DPL3 ; 显存首地址

     GdtLen                  equ             $ - LABEL_GDT
     GdtPtr                  dw              GdtLen - 1

                        dd              BaseOfLoaderPhyAddr + LABEL_GDT


     ;GDT Selector

     SelectorFlatC           equ             LABEL_DESC_FLAT_C       - LABEL_GDT
     SelectorFlatRW          equ             LABEL_DESC_FLAT_RW      - LABEL_GDT
     SelectorVideo           equ             LABEL_DESC_VIDEO        - LABEL_GDT + SA_RPL3

     ;2016.3.6
     ;BaseOfLoader          equ          09000h
     ;OffsetOfLoader        equ          0100h
     
     BaseOfLoaderFile           equ          09000h
     OffsetOfLoaderFile         equ          0100h
     BaseOfLoaderFileEntry      equ          08000h
     OffsetOfLoaderFileEntry    equ          0100h
     BaseOfLoaderPhyAddr    equ		BaseOfLoaderFile*10h 
     BaseOfKernelFile	    equ		05000h	;读取的地址空间太大，溢出了，将 07000h 改为了 06000h
     OffsetOfKernelFile	    equ		0h
     BaseOfKernelPhyAddr	equ	BaseOfKernelFile*10h
     KernelEntryPointPhyAddr	equ	0h	; 
     loops                   dd     2
 
START:
          mov     ax, cs
          mov     ss, ax
          mov	  ds, ax
	  mov     es, ax
	  mov	  fs, ax
	  mov	  gs, ax
	  mov     sp, 0xFFFF
  
          ;mov     ax, 0600h
          ;mov     bx, 0700h
          ;mov     cx, 0
          ;mov     dx, 184fh
          ;int     10h
	  ;get the memory size
  	  mov	ebx, 0
	  mov	di, _MemChkBuf
.MemChkLoop:
	  mov	eax, 0E820h
	  mov	ecx, 20
	  mov	edx, 0534D4150h
	  int	15h
	  jc	.MemChkFail
	  add	di, 20
	  inc	dword[_dwMCRNumber]
	  cmp	ebx, 0
	  jne	.MemChkLoop
	  jmp	.MemChkOK
.MemChkFail:
	  mov	dword[_dwMCRNumber], 0
.MemChkOK:
	 ;mov	ax, 0x0800;       ;8*256 - 1 = 2047
	  mov   ax, 3616
          mov   cx, 0x1;
	  push  ax
          mov   ax, BaseOfLoaderFileEntry
          mov   es, ax
          mov   bx, OffsetOfLoaderFileEntry
          pop   ax
	  call	ReadSectors
          
          mov   si, LoaderFileName      ;"LOADER  BIN"
          mov   di, OffsetOfLoaderFileEntry
          ;inc   di
          add	di, 32
	  cld
          mov   dx, 8
          .label_search_loaderbin:
          	cmp dx, 0
		jz .label_no_loaderbin
		dec dx
		
		mov cx, 11 
		.label_cmp_filename:
			cmp cx, 0
			jz .label_filename_found
			dec cx
			lodsb
			cmp al, byte[es:di]
			jz .label_go_on
			jmp .label_different
		.label_go_on:
			inc di
			jmp .label_cmp_filename
		.label_different:
			and di, 0ff00h
                        add di, 32
                        inc di
                        mov si, LoaderFileName
			jmp .label_search_loaderbin

	.label_no_loaderbin:
		call DisplayNotFound
		jmp $

	.label_filename_found:
		call	DisplayMessage 
		mov	ax, 3616;  this sector did not read into memeory
          	mov   cx, 0x1;
          	push  ax
          	mov   ax, BaseOfLoaderFileEntry
          	mov   es, ax
          	mov   bx, OffsetOfLoaderFileEntry
          	pop   ax
          	call  ReadSectors
	          mov   di, OffsetOfLoaderFileEntry
          	add   di, 32
		
		;and	di, 0FFF0h
		push	eax
		mov	eax, [es:di + 01Ch]
		mov	dword[dwKernelSize], eax; KERNEL.BIN size
		pop	eax
		;push	eax
		;mov	eax, [es:di + 018h]
		;mov	dword[KernelEntryPointPhyAddr], eax ; elf入口地址
		;pop	eax
		push	bx
		mov	bl, [es:di + 01Ah]
		mov	bh, [es:di + 014h]
		
		mov	cx, bx
		pop	bx
		add	cx, ax;  here cx is 0x00000003
		sub	cx, 2 ;  cx + ax -2 the kernel.bin file location 
			      ;  in this program is 3617
		mov	ax, BaseOfKernelFile
		mov	es, ax  ; es <- BaseOfKernel
		mov	bx, OffsetOfKernelFile; bx <- OffsetOfKernel
		mov	ax, cx  ; ax <- Sector
		;2016.4.4
		mov	ecx, dword[dwKernelSize]
		;2016.4.5 cacluate the sectors of kernel file
		shr	ecx, 9
		add	ecx, 1
		; 5.22 修改es
		mov	edx, 1
		.loop:
			push	ax
			push	bx
			mov	ah, 0Eh; //每读0x80个扇区打一个点
			mov	al, '.'
			mov	bl, 0Fh
			int	10h
			pop	bx
			pop	ax
		;*************************分割线****************************
			;cmp	dx, 0  ;只读两个0x80个扇区
			mov	cl, 0x80;每次读 0x80个扇区
			push	ax
			call	ReadSectors		
			pop	ax

			add	ax, 0x80	
			push	cx
			mov	cx, 6000h	;es溢出在 0x60000h 添加0x80个扇区			
			mov	es, cx
			pop	cx
			mov	cl, 0x80
			mov	bx ,0
			push	ax
			call	ReadSectors
			pop	ax

			add	ax, 0x80	
			push	cx
			mov	cx, 7000h	;es溢出在 0x70000h 添加0x80个扇区			
			mov	es, cx
			pop	cx
			mov	cl, 0x80
			mov	bx ,0
			push	ax
			call	ReadSectors
			pop	ax
						; 总共3 * 0x80个扇区,防止kernel.bin太大,加载不进来

			jmp	.next
		.next:
			mov	di, OffsetOfKernelFile
			call	.label_file_loaded
		; 5.22 修改es注释的	
		;mov	edx, ecx
		;.loop:
		;	; 每读一个扇区打一个点
		;	push	ax
		;	push	bx
		;	mov	ah, 0Eh
		;	mov	al, '.'
		;	mov	bl, 0Fh
		;	int	10h
		;	pop	bx
		;	pop	ax
		;	; 扇区都读了，为啥没数据？
		;	cmp dx, 0
		;	jz .next
		;	dec dx
		;	mov cl, 1
		;	push ax
		;	call	ReadSectors
		;	add bx, 512
		;	pop ax
		;	inc ax
		;	jmp	.loop
		;.next:		
		;	mov	di, OffsetOfKernelFile
		;	call	.label_file_loaded

	.label_file_loaded:
		call	KillMotor
		mov	dh, 1
		call	DispStrRealMode		
		;jmp	to protect mode
		;load	GDTR
		lgdt	[GdtPtr]
		
		cli
		;A20 location
		in	al, 92h
		or	al, 00000010b
		out	92h, al
		mov	eax, cr0
		or	eax, 1
		mov	cr0, eax
		

		jmp   dword SelectorFlatC:(BaseOfLoaderPhyAddr+LABEL_PM_START)

         

	 ;jmp   BaseOfLoader:OffsetOfLoader
	  ;call	DisplayMessage
	;当前部分执行是错误发生
;          mov     ah, 0x00
;         int     0x19                                ; 启动

     ;**********************************************************
     ;KillMotor
     ;*********************************************************
     KillMotor:	
		push	dx
		mov	dx, 03F2h
		mov	al, 0
		out	dx, al
		pop	dx
		ret		
     ;********************************************************************
     ;DispStrRealMode
     ;**********************************************************************

     DispStrRealMode:
          push    ax
          push    bp
          push    cx
          push    bx
          push    dx
          mov     ax, cs
          mov     es, ax

          mov     ax, KernelMessage
          mov     bp, ax
          mov     cx, 14
          mov     ax, 01301h                            ; BIOS teletype
          mov     bx, 000ch                            ; display page 0
          mov     dl, 0
          int     0x10

          pop     dx
          pop     bx
          pop     dx
          pop     bp
          pop     ax
          ret
     ;*************************************************************************
     ; ReadSectors
     ; 从ax个Sector地址处读取cx 个Sector到内存es:bx处
     ;*************************************************************************
ReadSectors:
    push bp
    mov bp, sp
    sub esp, 2                     ;两个字节的局部变量，用于保存要读取的扇区数
    
    mov byte[bp-2], cl             
    push ax
    push bx
    push cx
    push dx
   
   ;计算柱面号，起始扇区，磁头号
   ;4124 / 63 商65余29
   ;扇区号:30
   ;磁头号:1
   ;磁道号:4
   ;ax / 63
    push bx
    mov bl, 63                     ;
    div bl                         ; 
    inc ah
    mov cl, ah
    mov dh, al
    shr al, 4
    mov ch, al
    and dh, 0x0F
    ;mov ch, 4                     ;磁道号的低8位,
    ;mov dh, 1                     ;磁头号
    ;inc al                        ;逻辑扇区号从0开始编号，加1
    ;mov cl, 30                    ;cl低5位放入所读起始扇区号，7-6位表示磁道号的高2位
    mov dl, 80h                    ;驱动器号，80h表示硬盘
                                   ;4，1，30
    .reading:
        mov ah, 2                  ;BIOS int13h中断的2号子程序
        mov al, byte[bp-2]         ;al为要读取的扇区数
        pop bx
        int 13h                    ;前面设置好了对应的寄存器，现在掉用13号中断
        jc .reading                ;如果读取错误CF会被置为1，这时就不停地读，直到正确

    pop dx
    pop cx
    pop bx
    pop ax
    add esp, 2
    pop bp
    ret
;ReadSector结束
     
     ;*************************************************************************
     ; DisplayMessage
     ; 通过BIOS显示 ds:si 的ASCII码
     ;*************************************************************************
    	DisplayNotFound:
	  push    ax
	  push    bp
          push    cx
          push    bx
          push    dx
          mov     ax, cs
          mov     es, ax

          mov     ax, LoaderNotFound
	  mov	  bp, ax
          mov     cx, 9
          mov     ax, 01301h                            ; BIOS teletype
          mov     bx, 000ch                            ; display page 0
          mov     dl, 0
          int     0x10

          pop     dx
          pop     bx
          pop     dx
          pop     bp
          pop     ax
          ret
	
	DisplayMessage:
          push    ax
          push    bp
          push    cx
          push    bx
          push    dx
          mov     ax, cs
          mov     es, ax

          mov	  ax, BootMessage
	  mov	  bp, ax
	  mov	  cx, 8
          mov     ax, 01301h                            ; BIOS teletype
          mov     bx, 000ch                            ; display page 0
          mov	  dl, 0
	  int     0x10
 
          pop     dx
          pop     bx
          pop     dx
          pop     bp
          pop     ax                                
    	  ret 
     ;*************************************************************************


     ; **************************************************
     ; LBACHS
     ; 将ax 个sector的LBA地址转换成CHS地址
     ; absolut sector = (logical sector / sectors per track) + 1
     ; absolut head   = (logical sector / sectors per track) MOD number of heads
     ; absolut track  = logical sector / (sectors per track * number of heads)
     ;*************************************************************************
     LBACHS:
          ; dx, ax, bx
          push    ax
          push    bx
          push    cx
          push    dx

          push	  bx
	  xor     dx, dx                              ; dx:ax 使用
	  ;mov	  dx, 0x0020
          mov     bx, 63
	  div	  bx
	  pop     bx
	  mov	  cx, dx
	  inc     cx
	  and	  cl, 3FH	;cl = sector	                                
          xor     dx, dx                              
          ;mov	  dx, 0
	  push	  bx
	  xor     bx, bx
	  ;mov     bx, 0x0040
	  mov     bx, 16
          div     bx
	  mov	  ch, al ; cylinder low 8 bit
	  mov     BYTE [CylinderLow], ch
	  shl     ah, 6
	  or      cl, ah ; cl = cylinder high 2 bit : 6 bit sector              
          mov	  BYTE [CylinderHS], cl
	  mov     BYTE [Headers], dl
          pop	  bx

          pop     dx
          pop     cx
          pop     bx
          pop     ax
	  ret
      ;************************************************************************
     CylinderLow db 0x00
     CylinderHS   db 0x00
     Headers  db 0x00
     cluster     dw 0x0000
     datasector  dw 0x0000
     dwKernelSize       dw 0x0000
     LoaderFileName     db      "KERNEL  BIN"
     BootMessage        db      "KERNELBIN"
     LoaderNotFound     db      "NOT FOUND"
     KernelMessage      db      "kernel loaded!"
     MessageLength      equ     9
;     times 2558-($-$$) db 0
;     dw      0xaa55


[SECTION .s32]
ALIGN	32
[BITS	32]
[extern start]
LABEL_PM_START:
	
	mov	ax, SelectorVideo
	mov	gs, ax
	
	mov	ax, SelectorFlatRW
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	ss, ax
	mov	esp, TopOfStack  ;0x000604a0
	
        mov     ah, 0Ch                         ; 0000: 黑底    1111: 白字
        mov     al, 'K'
        mov     [gs:((80 * 0 + 39) * 2)], ax    ; 屏幕第 0 行, 第 39 列。
	call	InitKernel
	;jmp 	SelectorFlatC:KernelEntryPointPhyAddr 
	jmp 	SelectorFlatC:1001e0H
	jmp     $

;*******************************************************************
;Initkernel
;put the right values ofprogram header
;******************************************************************
InitKernel:

        xor   esi, esi
        mov   cx, word [BaseOfKernelPhyAddr+2Ch];`. ecx <- pELFHdr->e_phnum 0x3
        movzx ecx, cx                               ;/
	push  eax
	mov   eax, dword[BaseOfKernelPhyAddr + 18h]
	mov   dword[KernelEntryPointPhyAddr], eax  ; kernelEntryPointPhyAddr 0x0010005ch
	pop   eax
        mov   esi, [BaseOfKernelPhyAddr + 1Ch]	  		; esi <- pELFHdr->e_phoff 0x34
        add   esi, BaseOfKernelPhyAddr				;esi<-OffsetOfKernel+pELFHdr->e_phoff 0x60034
.Begin:
        mov   eax, [esi + 0]		;0x1
        cmp   eax, 0                      ; PT_NULL
        jz    .NoAction
        push  dword [esi + 010h]    ;size ;`. 0x0000 0400
        mov   eax, [esi + 04h]            ; | 0x0000 1000 offset
        add   eax, BaseOfKernelPhyAddr; | memcpy((void*)(pPHdr->p_vaddr)0x6 1000
        push  eax		    ;src  ; |      uchCode + pPHdr->p_offset,
        push  dword [esi + 08h]     ;dst  ; |      pPHdr->p_filesz; 0x1 0000
        call  MemCpy                      ; |
        add   esp, 12                     ;/
.NoAction:
        add   esi, 020h                   ; esi += pELFHdr->e_phentsize
        dec   ecx
        jnz   .Begin
	
	mov     ah, 0Ch                         ; 0000: 黑底    1111: 白字
        mov     al, 'M'
        mov     [gs:((80 * 5 + 39) * 2)], ax    ; 屏幕第 0 行, 第 39 列。


        ret
; InitKernel ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^



;*****************************************************************

;*******************************************************************
;Memcpy like the c program memcpy()
;********************************************************************
MemCpy:
	push	ebp
	mov	ebp, esp	;0x 61000

	push	esi
	push	edi
	push	ecx

	mov	edi, [ebp + 8]	; Destination ;0x10 0000
	mov	esi, [ebp + 12]	; Source      ;0x61 0000
	mov	ecx, [ebp + 16]	; Counter     ;0x0 4000
.1:
	cmp	ecx, 0		; 判断计数器
	jz	.2		; 计数器为零时跳出

	mov	al, [ds:esi]		; ┓
	inc	esi			; ┃
					; ┣ 逐字节移动
	mov	byte [es:edi], al	; ┃
	inc	edi			; ┛

	dec	ecx		; 计数器减一
	jmp	.1		; 循环
.2:
	mov	eax, [ebp + 8]	; 返回值

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			; 函数结束，返回

;*******************************************************************


;**************************************************************************
[section .data1]
ALIGN	32
label_data:
     TopOfStack      equ     BaseOfKernelPhyAddr + $
     _dwMCRNumber	dd	0
     _dwMemSize		dd	0
     _MemChkBuf:		times	256	db	0
  ;*************************************************************************
