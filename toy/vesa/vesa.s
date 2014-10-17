;init/[1,7]
[BITS 64]

    mov rdi,[0x3028]
    xor eax,eax

    mov ecx,0x10000
blue:
    call point
    add eax,0x000001
    loop blue


    xor eax,eax
    mov ecx,0x10000
green:
    call point
    add eax,0x000100
    loop green


    xor eax,eax
    mov ecx,0x10000
red:
    call point
    add eax,0x010000
    loop red


hlt:hlt
    jmp hlt


point:
    stosd
    dec rdi
    ret
