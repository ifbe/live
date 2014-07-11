bits 64
section .text

extern fat16_cd
extern fat16_load
global finishfat16

extern fat32_cd
extern fat32_load
global finishfat32

extern ntfs_cd
extern ntfs_load
global finishntfs

extern ext_cd
extern ext_load
global finishext




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




finishntfs:
mov rdi,0x7020
mov rax,"cd"
stosq
lea rax,[rel ntfs_cd]
stosq
mov rax,"load"
stosq
lea rax,[rel ntfs_load]
stosq

ret





finishext:
mov rdi,0x7020
mov rax,"cd"
stosq
lea rax,[rel ext_cd]
stosq
mov rax,"load"
stosq
lea rax,[rel ext_load]
stosq

ret
