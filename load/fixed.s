[ORG 0x7c00]              ; add to offsets
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   cli                    ; no interrupts
   xor ax, ax             ; make it zero
   mov ds,ax
   mov es,ax
   mov ss,ax             ; stack starts at seg 0
   mov sp, 0x7c00         ; 2000h past code start, 

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
loadinit:
    ;mov cx,8            ;[10000,8ffff]
    mov cx,2            ;[10000,8ffff]
    mov eax,128
    mov bx,0x1000
continue:
    mov dword [dap.blocknumber],eax
    mov byte [dap.blockcount],64
    mov word [dap.buffersegment],bx
    mov word [dap.bufferoffset],0x0000
    call loadsector
    add eax,64

    mov dword [dap.blocknumber],eax
    mov byte [dap.blockcount],64
    mov word [dap.bufferoffset],0x8000
    mov word [dap.buffersegment],bx
    call loadsector
    add eax,64

    add bx,0x1000

    loop continue

    jmp 0x1000:0x0000
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
db 0x80,0xfe,0xff,0xff,0x0b,0xfe,0xff,0xff,0x21,0x00,0x00,0x00,0xff,0x03,0x00,0x00
times 48 db 0
dw 0xaa55
