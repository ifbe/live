bits 64
section .text

extern ntfs_cd
extern ntfs_load

global finishntfs





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
