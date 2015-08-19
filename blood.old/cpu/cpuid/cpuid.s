;init/[1,6]
[BITS 64]
cpuid0:
xor rax,rax
cpuid
mov edi,0xb8000
mov rax,0xf0f0f0f0f0f0f0f0
stosq
stosq
stosq
mov edi,0xb8000

mov [edi],bl
add edi,2
shr ebx,8
mov [edi],bl
add edi,2
shr ebx,8
mov [edi],bl
add edi,2
shr ebx,8
mov [edi],bl
add edi,2

mov [edi],dl
add edi,2
shr edx,8
mov [edi],dl
add edi,2
shr edx,8
mov [edi],dl
add edi,2
shr edx,8
mov [edi],dl
add edi,2

mov [edi],cl
add edi,2
shr ecx,8
mov [edi],cl
add edi,2
shr ecx,8
mov [edi],cl
add edi,2
shr ecx,8
mov [edi],cl


cpuid1:
mov edi,0xb8140
mov rax,0x7020705870417052
stosq
add edi,24
mov rax,0x7020705870427052
stosq
add edi,24
mov rax,0x7020705870437052
stosq
add edi,24
mov rax,0x7020705870447052
stosq
add edi,24

mov rax,1
cpuid                    ;edx(primary),ecx(external)
mov edi,0xb81e0
call dumper

death:hlt
jmp death

dumper:
bswap rax
mov [datarax],rax
bswap rbx
mov [datarbx],rbx
bswap rcx
mov [datarcx],rcx
bswap rdx
mov [datardx],rdx

mov rsi,datarax
mov rcx,32

dumpchar:
mov al,[esi]
shr al,4
add al,0x30
cmp al,0x3a
jb .firstdone
add al,0x7
.firstdone:
stosb
mov al,0x70
stosb

lodsb
and al,0x0f
add al,0x30
cmp al,0x3a
jb .seconddone
add al,0x7
.seconddone:
stosb
mov al,0x70
stosb

loop dumpchar
ret

datarax dq 0
datarbx dq 0
datarcx dq 0
datardx dq 0
