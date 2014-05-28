bits 64
section .text

extern initahci
extern parttable
extern mount
extern say
extern read
extern fat16_cd
extern fat16_load
extern fat32_cd
extern fat32_load
extern getdisk

global start
global where
global incwhere
global cleanwhere
global fat16
global fat16root
global fat32
global fat32root
global clustersize
global cluster0
global fat0








start:

mov edi,0x40000
xor eax,eax
mov ecx,0x10000
cld
rep stosd

call initahci
call parttable
call mount
ret








fat16:
lea rdi,[rel saypartition]	;varieties
mov esi,[0x12001c]
mov [rel realpartition],rsi
call say

lea rdi,[rel sayfatsize]
movzx rsi,word [0x120016]
mov [rel realfatsize],rsi
call say

lea rdi,[rel sayfat0]
movzx rsi,word [0x12000e]
add rsi,[rel realpartition]
mov [rel realfat0],rsi
call say

lea rdi,[rel sayclustersize]
movzx rsi,byte [0x12000d]
mov [rel realclustersize],rsi
call say

lea rdi,[rel saycluster0]
mov rsi,[rel realfat0]
add rsi,[rel realfatsize]
add rsi,[rel realfatsize]
add rsi,32
sub rsi,[rel realclustersize]
sub rsi,[rel realclustersize]
mov [rel realcluster0],rsi
call say

fat16root:
call getdisk
mov rdx,rax
mov rdi,0x120000			;[120000]=root
mov rsi,[rel realfat0]
add rsi,[rel realfatsize]
add rsi,[rel realfatsize]
mov rcx,32
call read

lea rdi,[rel saycdroot]
mov rsi,[rel realfat0]
add rsi,[rel realfatsize]
add rsi,[rel realfatsize]
call say

mov rdi,0x7000			;[7000]=function address
mov rax,"cd"
stosq
lea rax,[rel fat16_cd]
stosq
mov rax,"load"
stosq
lea rax,[rel fat16_load]
stosq

ret







fat32:
lea rdi,[rel saypartition]
mov esi,[0x12001c]
mov [rel realpartition],rsi
call say

lea rdi,[rel sayfatsize]
mov esi,[0x120024]
mov [rel realfatsize],rsi
call say

lea rdi,[rel sayfat0]
movzx rsi,word [0x12000e]
add rsi,[rel realpartition]
mov [rel realfat0],rsi
call say

lea rdi,[rel sayclustersize]
movzx rsi,byte [0x12000d]
mov [rel realclustersize],rsi
call say

lea rdi,[rel saycluster0]
mov rsi,[rel realfat0]
add rsi,[rel realfatsize]
add rsi,[rel realfatsize]
sub rsi,[rel realclustersize]
sub rsi,[rel realclustersize]
mov [rel realcluster0],rsi
call say

fat32root:
call getdisk
mov rdx,rax
mov rdi,0x120000
mov rsi,[rel realcluster0]
add rsi,[rel realclustersize]
add rsi,[rel realclustersize]
mov rcx,32
xor eax,eax
call read

lea rdi,[rel saycdroot]
mov rsi,[rel realcluster0]
add rsi,[rel realclustersize]
add rsi,[rel realclustersize]
call say

mov rdi,0x7000			;[7000]=function address
mov rax,"cd"
stosq
lea rax,[rel fat32_cd]
stosq
mov rax,"load"
stosq
lea rax,[rel fat32_load]
stosq

ret








fat0:
mov rax,[rel realfat0]
ret

clustersize:
mov rax,[rel realclustersize]
ret

cluster0:
mov rax,[rel realcluster0]
ret








realpartition:dq 0
realfatsize:dq 0
realfat0:dq 0
realclustersize:dq 0
realcluster0:dq 0

saypartition:db "partition:",0
sayfatsize:db "fatsize:",0
sayfat0:db "fat0:",0
sayclustersize:db "clustersize:",0
saycluster0:db "cluster0:",0
saycdroot:db "cd root:",0
