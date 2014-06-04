bits 64
section .text

extern fat16_cd
extern fat16_load
extern fat32_cd
extern fat32_load
global finishfat16
global finishfat32




finishfat16:
mov rdi,0x7020			;[7000]=function address
mov rax,"cd"
stosq
lea rax,[rel fat16_cd]
stosq
mov rax,"load"
stosq
lea rax,[rel fat16_load]
stosq

ret







finishfat32:

fat32root:

mov rdi,0x7020			;[7000]=function address
mov rax,"cd"
stosq
lea rax,[rel fat32_cd]
stosq
mov rax,"load"
stosq
lea rax,[rel fat32_load]
stosq

ret
