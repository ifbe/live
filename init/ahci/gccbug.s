bits 64
section .text
extern say
extern read
global finddisk
global disk
global where
global incwhere
global cleanwhere
global fat16
global fat32
global clustersize
global cluster0
global fat0


finddisk:
mov esi,0x2808
.loop:
cmp dword [esi],"sata"
je .find
add esi,0x10
cmp esi,0x3000
ja .return
jmp .loop
.find:
mov rax,[esi-8]
mov [rel realdisk],rax
.return:
lea edi,[rel saydisk]
mov rsi,[rel realdisk]
call say
ret




disk:
mov rax,[rel realdisk]
ret

saydisk:db "disk:",0
realdisk:dq 0




incwhere:
inc byte [rel realwhere]
ret

cleanwhere:
mov qword [rel realwhere],0
ret

where:
mov rax,[rel realwhere]
ret

realwhere:
dq 0




fat16:
lea rdi,[rel saypartition]
mov esi,[0x10001c]
mov [rel realpartition],rsi
call say

lea rdi,[rel sayfatsize]
movzx rsi,word [0x100016]
mov [rel realfatsize],rsi
call say

lea rdi,[rel sayfat0]
movzx rsi,word [0x10000e]
add rsi,[rel realpartition]
mov [rel realfat0],rsi
call say

lea rdi,[rel sayclustersize]
movzx rsi,byte [0x10000d]
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

mov rdi,0x100000
mov rsi,[rel realfat0]
add rsi,[rel realfatsize]
add rsi,[rel realfatsize]
call disk
mov rdx,rax
mov rcx,32
call read

lea rdi,[rel saycdroot]
mov rsi,[rel realfat0]
add rsi,[rel realfatsize]
add rsi,[rel realfatsize]
call say
ret

saycdroot:db "cd root",0

fat32:
lea rdi,[rel saypartition]
mov esi,[0x10001c]
mov [rel realpartition],rsi
call say

lea rdi,[rel sayfatsize]
mov esi,[0x100024]
mov [rel realfatsize],rsi
call say

lea rdi,[rel sayfat0]
movzx rsi,word [0x10000e]
add rsi,[rel realpartition]
mov [rel realfat0],rsi
call say

lea rdi,[rel sayclustersize]
movzx rsi,byte [0x10000d]
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

mov rdi,0x100000
mov rsi,[rel realcluster0]
add rsi,[rel realclustersize]
add rsi,[rel realclustersize]
call disk
mov rdx,rax
mov rcx,32
call read

lea rdi,[rel saycdroot]
mov rsi,[rel realcluster0]
add rsi,[rel realclustersize]
add rsi,[rel realclustersize]
call say
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
