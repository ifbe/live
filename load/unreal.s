[ORG 0x7c00]              ; add to offsets
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   cli                    ; no interrupts
   xor ax, ax             ; make it zero
   mov ds,ax
   mov es,ax
   mov ss,ax             ; stack starts at seg 0
   mov sp, 0x7c00         ; 2000h past code start, 

   in al,0x92
   or al,0x2
   out 0x92,al
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
unrealmode:
   push ds 
   push es
   lgdt [fakegdt]         ; load gdt register
 
   mov  eax, cr0          ; switch to pmode by
   or al,1                ; set pmode bit
   mov  cr0, eax
 
   mov bx,0x10
   mov ds,bx
   mov es,bx
 
   and al,0xFE            ; back to realmode
   mov  cr0, eax          ; by toggling bit again

   pop es
   pop ds 

   jmp loadinit
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


;<<<<<<<<<<<<<<<<<<<<<fake gdt<<<<<<<<<<<<<<<<<<<<<<
fakegdt:
   dw flatend - flatgdt - 1   ;last byte in table
   dd flatgdt                 ;start of table
 
flatgdt         dd 0,0        ; entry 0 is always unused
flatcode    db 0xff, 0xff, 0, 0, 0, 0x9a, 0x8f, 0
flatdata    db 0xff, 0xff, 0, 0, 0, 0x92, 0xcf, 0
flatend:
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;    mov dword [dap.blocknumber],96
;    mov byte [dap.blockcount],32
;    mov word [dap.bufferoffset],0xc000
;    call loadsector

loadinit:
    mov cx,8            ;5:[10000,7ffff]
    mov eax,128
    mov bx,0x1000
continue:
    mov word [dap.buffersegment],bx

    mov dword [dap.blocknumber],eax
    mov byte [dap.blockcount],64
    mov word [dap.bufferoffset],0x0000
    call loadsector
    add eax,64

    mov dword [dap.blocknumber],eax
    mov byte [dap.blockcount],64
    mov word [dap.bufferoffset],0x8000
    call loadsector
    add eax,64

    add bx,0x1000
    loop continue

;movejpeg:
;    mov esi,0x20000
;    mov edi,0x200000
;    mov ecx,0x18000
;    a32 rep movsd

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
