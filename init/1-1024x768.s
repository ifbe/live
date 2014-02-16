ORG 0x10000
BITS 16
;_________________环境设置__________________
    cli
    xor ax,ax    ; Set up segment registers.
    mov ss,ax    ; Set up stack so that it starts below Main.
    mov sp,0x7c00
    mov ds,ax
    mov es,ax
    cld

    xor eax,eax
    mov di,0x8000
    mov cx,0x1000
    rep stosd
;__________________________________________


;____________int15 e820 memory detect___________
    mov di,0x8800
    xor ebx,ebx
e820:
    mov eax,0xe820
    mov ecx,24
    mov edx,0x534d4150
    int 0x15
    jc e820finish
    cmp eax,0x534d4150
    jne e820finish
    cmp di,0x9000
    ja e820finish
    add di,0x20
    cmp ebx,0
    jne e820

e820finish:
;_____________________________________________


;____________read cmos/disable nmi_____________
    mov di,0x500
cmos:
    lea ax,[di-0x480]
    out 0x70,al
    in al,0x71
    stosb
    cmp di,0x580
    jb cmos
;_____________________________________________


;______________列出vesa模式________________
    mov cx,0x101
    mov di,0xa020

listresolution:
    mov ax,0x4f01
    int 0x10         ;changed:ax
    add di,0x20
    inc cx
    cmp cx,0x180
    jb listresolution
;_____________________________________________


;_________________________________
    mov si,0xa000
searchresolution:
    add si,0x20
    cmp si,0xafff
    ja noresolution
    mov eax,[si+0x12]
    cmp eax,0x03000400
    jne searchresolution
    cmp byte [si+0x19],0x18
    jb searchresolution
;__________________________________


;__________________________________
setresolution:
    sub si,0xa000
    shr si,5
    push si

    mov cx,si
    mov ch,1
    mov di,0xa000
    mov ax,0x4f01
    int 0x10

    pop bx
    mov bh,0x41
    mov ax,0x4f02
    int 0x10
;__________________________________
noresolution:
