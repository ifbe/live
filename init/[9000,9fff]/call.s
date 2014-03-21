[bits 64]
startoffakekernel:

mov rdx,0xc000
call rdx
mov rdx,0xd000
call rdx
mov rdx,0x10000
call rdx
sleep:hlt
jmp sleep

endoffakekernel:
times 0x1000-(endoffakekernel-startoffakekernel) db 0
