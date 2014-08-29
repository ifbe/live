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




findfreespace:
mov edi,0x4000
.continue:
cmp qword [edi],0
je .return
add edi,0x10
jmp .continue

.return:
ret




finishfat16:
call findfreespace
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

call findfreespace
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
call findfreespace
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
call findfreespace
mov rax,"cd"
stosq
lea rax,[rel ext_cd]
stosq
mov rax,"load"
stosq
lea rax,[rel ext_load]
stosq

ret
