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

global start
global finddisk
global disk
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
mov edi,0x8000
xor rax,rax
mov ecx,0x1000
rep stosq

call initahci
call finddisk
call parttable
call mount
ret








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
lea rdi,[rel saypartition]	;varieties
mov esi,[0x8001c]
mov [rel realpartition],rsi
call say

lea rdi,[rel sayfatsize]
movzx rsi,word [0x80016]
mov [rel realfatsize],rsi
call say

lea rdi,[rel sayfat0]
movzx rsi,word [0x8000e]
add rsi,[rel realpartition]
mov [rel realfat0],rsi
call say

lea rdi,[rel sayclustersize]
movzx rsi,byte [0x8000d]
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
mov rdi,0x80000			;[80000]=root
mov rsi,[rel realfat0]
add rsi,[rel realfatsize]
add rsi,[rel realfatsize]
mov rdx,[rel realdisk]
mov rcx,32
call read

lea rdi,[rel saycdroot]
mov rsi,[rel realfat0]
add rsi,[rel realfatsize]
add rsi,[rel realfatsize]
call say

mov rdi,0x8000			;[8000]=function address
lea rax,[rel fat16_cd]
stosq
mov rax,"cd"
stosq
lea rax,[rel fat16_load]
stosq
mov rax,"load"
stosq

ret







fat32:
lea rdi,[rel saypartition]
mov esi,[0x8001c]
mov [rel realpartition],rsi
call say

lea rdi,[rel sayfatsize]
mov esi,[0x80024]
mov [rel realfatsize],rsi
call say

lea rdi,[rel sayfat0]
movzx rsi,word [0x8000e]
add rsi,[rel realpartition]
mov [rel realfat0],rsi
call say

lea rdi,[rel sayclustersize]
movzx rsi,byte [0x8000d]
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
mov rdi,0x80000
mov rsi,[rel realcluster0]
add rsi,[rel realclustersize]
add rsi,[rel realclustersize]
mov rdx,[rel realdisk]
mov rcx,32
xor eax,eax
call read

lea rdi,[rel saycdroot]
mov rsi,[rel realcluster0]
add rsi,[rel realclustersize]
add rsi,[rel realclustersize]
call say

mov rdi,0x8000			;[8000]=function address
lea rax,[rel fat32_cd]
stosq
mov rax,"cd"
stosq
lea rax,[rel fat32_load]
stosq
mov rax,"load"
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
