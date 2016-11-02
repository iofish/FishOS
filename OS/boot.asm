;*************************************************************************
; fishos kernel boot.asm for the first sector
;*************************************************************************
[BITS 16]
org 07c00h
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

     ;2016.3.6
     BaseOfLoader          equ          09000h
     OffsetOfLoader        equ          0100h

START:
          cli
     ; 创建栈
          mov     ss, ax
          mov	  dx, ax
	  mov     es, ax
	  mov	  fs, ax
	  mov	  gs, ax
	  mov     sp, 0xFFFF
          sti

          mov     ax, 0600h
          mov     bx, 0700h
          mov     cx, 0
          mov     dx, 184fh
          int     10h

   	  mov	ax, 0x0800;       ;8*256 - 1 = 2047
	  mov   cx, 0x5;
	  push  ax
          mov   ax, BaseOfLoader
          mov   es, ax
          mov   bx, OffsetOfLoader
          pop   ax
	  call	ReadSectors
	  call  DisplayMessage
          jmp   BaseOfLoader:OffsetOfLoader
	  call	DisplayMessage
	;当前部分执行是错误发生
          mov     ah, 0x00
          int     0x19                                ; 启动

     ;*************************************************************************
     ; ReadSectors
     ; 从ax个Sector地址处读取cx 个Sector到内存es:bx处
     ;*************************************************************************
     ReadSectors:
     .MAIN:
          push    si

     .SECTORLOOP:
          push    ax
          push    bx
          push    cx
          call    LBACHS
          mov     ah, 0x02                            ; BIOS read sector
          mov     al, cl                            ; read one sector
          mov     ch, BYTE [CylinderLow]            ; track
          mov     cl, BYTE [CylinderHS]           ; sector
          mov     dh, BYTE [Headers]             ; head
          mov     dl, 0x80              ; drive
          int     0x13                                ; invoke BIOS
          pop     cx
          pop     bx
          pop     ax
          pop     si
          ret
   
	  jnc     .SUCCESS                            ; test for read error
          xor     ax, ax                              ; BIOS reset disk
          int     0x13                                ; invoke BIOS
         
          dec     di                                  ; decrement error counter
          pop     cx
          pop     bx
          pop     ax
          jnz     .SECTORLOOP                         ; attempt to read again
          int     0x18
     .SUCCESS:
          mov     si, msgProgress
          call    DisplayMessage
          pop     cx
          pop     bx
          pop     ax
          add     bx, WORD [BytesPerSector]           ; queue next buffer
          inc     ax                                  ; queue next sector
          loop    .MAIN                               ; read next sector
          ret

     
     ;*************************************************************************
     ; DisplayMessage
     ; 通过BIOS显示 ds:si 的ASCII码
     ;*************************************************************************
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
	  mov	  cx, 11
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



     ;*************************************************************************
     ; ClusterLBA
     ; 将FAT簇转换成LBA(逻辑区块地址)
     ; FileStartSector = ((X − 2) * SectorsPerCluster(0x08))
     ;*************************************************************************
     ClusterLBA:
          sub     ax, 0x0002                          
          xor     cx, cx
          mov     cl, BYTE [SectorsPerCluster]        
          mul     cx
          add     ax, WORD [datasector]               
          ret

     ;*************************************************************************
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

	;*******************************************************************************
     BootMessage:	db	"FishOs Boot"
     msgProgress: db "FishOs.."
     msgFailure  db 0x0D, 0x0A, "Kernel loading failed...", 0x0D, 0x0A, 0x00
     msgCRLF     db "hello world", 0x0D, 0x0A, 0x00
     
     TIMES 510-($-$$) DB 0
     DW 0xAA55
     ;*************************************************************************
