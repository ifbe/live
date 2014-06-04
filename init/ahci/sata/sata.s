bits 64
section .text

extern getdisk
extern say
extern identify
extern read
global initsata





initsata:
mov rdi,0x7000			;[7000]=function address
mov rax,"identify"
stosq
lea rax,[rel directidentify]
stosq
mov rax,"read"
stosq
lea rax,[rel directread]
stosq

ret




directidentify:
call getdisk
mov rsi,rax
mov rdi,0x200000
call identify

mov edi,0x200000
mov ecx,0x100
.xxcchhgg:
mov ax,[edi]
xchg ah,al
stosw
loop .xxcchhgg

ret




directread:
push rsi

lea rdi,[rel readlog]
call say

call getdisk
mov rdx,rax
mov rdi,0x200000
pop rsi
mov rcx,0x400			;512K
call read

cmp eax,0
je .return

lea rdi,[rel readerror]
mov rsi,rax
call say

.return:
ret




readlog:db "reading sector:",0
readerror:db "something wrong:",0
