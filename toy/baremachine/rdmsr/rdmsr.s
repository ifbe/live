;init/[1,6]
[BITS 64]
mov rcx,0x1b
rdmsr
mov edi,0xb8000

mov rcx,8
dumpedx:
rol edx,4
mov bl,dl
and bl,0x0f
add bl,0x30
cmp bl,0x3a
jb .done
add bl,0x7
.done:
mov [edi],bl
inc edi
mov byte [edi],0xf0
inc edi
loop dumpedx


mov rcx,8
dumpeax:
rol eax,4
mov bl,al
and bl,0x0f
add bl,0x30
cmp bl,0x3a
jb .done
add bl,0x7
.done:
mov [edi],bl
inc edi
mov byte [edi],0xf0
inc edi
loop dumpeax

death:hlt
jmp death

