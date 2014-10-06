bits 64


;___________________________________________
;准备1维位置和偏移
    mov qword [rel addr],0x40000         ;r14 memory pointer
    mov qword [rel offset],0x420         ;r15 offset pointer

;准备二维xy坐标
    mov rax,"x"
    mov [0x4fc0],rax
    mov rax,512			;x坐标
    mov [0x4fc8],rax
    mov rax,"y"
    mov [0x4fd0],rax
    mov rax,256			;y坐标
    mov [0x4fd8],rax

;清空input buffer
    mov edi,0x800
    mov ecx,0x100
    xor rax,rax
    rep stosq
    mov rax,"input"
    mov [0xfe0],rax
    mov rax,0x800
    mov [0xfe8],rax
    mov rax,"progress"
    mov [0xff0],rax
    mov rax,0x800
    mov [0xff8],rax

;清空screen buffer
    mov edi,0x6000
    mov rax,"[   /]$ "
    stosq
    mov ecx,128*0x30-8
    xor rax,rax
    rep stosb

;准备进入正式程序
    lea rax,[rel function4]
    mov [rel screenwhat],rax
    lea rax,[rel menu]
    mov [rel mouseormenu],rax
    lea rax,[rel dumpanscii]
    mov [rel hexoranscii],rax

    xor rax,rax
    jmp decide
;____________________________________________________
