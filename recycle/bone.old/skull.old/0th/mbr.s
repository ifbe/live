[ORG 0x7c00]              ; add to offsets
startofmbr:
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   cli                    ; no interrupts
   xor ax, ax             ; make it zero
   mov ds,ax
   mov es,ax
   mov ss,ax             ; stack starts at seg 0
   mov sp, 0x7c00

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;这里判断一下，先读首扇区到一个临时的位置
    mov dword [dap.blocknumber],0
    mov byte [dap.blockcount],1
    mov word [dap.buffersegment],0
    mov word [dap.bufferoffset],0x800
    call loadsector
;跟这个文件前256个字节(后面256个会改变)不一样的话，就跳到本文件结尾
    mov esi,0x7c00
    mov edi,0x800
    mov ecx,0x100
    cld
    rep cmpsb
    jne endofmbr
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


;一样的话就是在虚拟环境里面，读前128个扇区到0x10000，然后跳过去
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
loadinit:
    xor eax,eax
    mov bx,0x1000
    ;mov cx,2            ;[10000,8ffff]
;continue:
    mov dword [dap.blocknumber],eax
    mov byte [dap.blockcount],64
    mov word [dap.buffersegment],bx
    mov word [dap.bufferoffset],0x0000
    call loadsector
    add eax,64

    mov dword [dap.blocknumber],eax
    mov byte [dap.blockcount],64
    mov word [dap.buffersegment],bx
    mov word [dap.bufferoffset],0x8000
    call loadsector
    add eax,64

    ;add bx,0x1000
    ;loop continue

    jmp 0x1000:0x0200
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


;<<<<<<<<<<<<<<load sector<<<<<<<<<<<<<<<<<<<<<<<
loadsector:
    pushad
    mov ah,0x42
    mov dl,0x80
    mov si,dap
    int 0x13
    popad
    ret
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


times 256-($-$$)db 0


;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
dap:
.packagesize db 0x10
.reserved db 0x00
.blockcount dw 0x0002
.bufferoffset dw 0xc000
.buffersegment dw 0x0000
.blocknumber dq 0x0000000000000001
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.


times 446-($-$$)db 0




db 0x80,0xfe,0xff,0xff
db 0x0b,0xfe,0xff,0xff
dd 0x800		;首扇区号
dd 0x1f800		;扇区数量


dd 0
dd 0
dd 0
dd 0


dd 0
dd 0
dd 0
dd 0


dd 0
dd 0
dd 0
dd 0


dw 0xaa55

endofmbr:
