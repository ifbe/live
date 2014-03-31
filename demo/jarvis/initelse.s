bits 64
;____________________________________
jpegprogram:
;    mov esi,0x30000
;    mov edi,0x100000
;    mov ecx,0x2000
;    rep movsq

;jpegdecode:                   ;位置无关
;    cli
;    mov rdx,0x100000            ;eax 传参
;    call rdx
;    mov [rel jpegbase],rax        ;return value
;    sti

prepare:
    mov r14,0x4000             ;r14 memory pointer
    xor r15,r15             ;r15 offset pointer

    mov rdi,0x800
    mov [0xff8],rdi

    xor rax,rax
    mov [0xff0],rax         ;keyboard buffer end

    mov ecx,0xfe
    rep stosq

    jmp ramdump
;_______________________________________
