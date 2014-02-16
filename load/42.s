ORG 0x7C00
BITS 16
    cli
    xor eax,eax    ; Set up segment registers.
    mov ds, ax
    mov es, ax
    mov ss, ax    ; Set up stack so that it starts below Main.
    mov sp, 0x7c00


;____________0xc000_____________________________
    mov eax,96
    mov [dap.blocknumber],eax
    mov byte [dap.blockcount],32
    mov word [dap.bufferoffset],0xc000
    call loadsector
;__________________________________________________


    jmp 0x0000:0xc000


;______________load sector_________________________
loadsector:
    pushad
    mov ah,0x42
    mov dl,0x80
    mov si,dap
    int 0x13
    popad
    ret
;_________________________________________________


dap:
.packagesize db 0x10
.reserved db 0x00
.blockcount dw 0x0002
.bufferoffset dw 0xc000
.buffersegment dw 0x0000
.blocknumber dq 0x0000000000000001

times 446-($-$$)db 0
db 0x80,0xfe,0xff,0xff,0x0b,0xfe,0xff,0xff,0x21,0x00,0x00,0x00,0xff,0x30,0x00,0x00
times 48 db 0
dw 0xaa55
