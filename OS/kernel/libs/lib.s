;导出函数
global port_read
global port_write
;=============================================================
;         void port_read(uint16_t port, void* buf, int n)
;=============================================================
port_read:
    mov edx, [esp+4]             ;port
    mov edi, [esp+4+4]           ;buf
    mov ecx, [esp+4+4+4]         ;n
    shr ecx, 1                   ;exc/2, 一个个字读取
    cld
    rep insw
    ret
;port_read结束


;=============================================================
;        void port_write(uint16_t port, void*buf, int n)
;=============================================================
port_write:
    mov edx, [esp+4]             ;port
    mov esi, [esp+4+4]           ;buf
    mov ecx, [esp+4+4+4]         ;n
    shr ecx, 1                   ;ecx/2, 一个个字写入
    rep outsw
    ret
