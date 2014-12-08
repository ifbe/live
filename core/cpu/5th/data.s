startofplacedata:

lea rsi,[rel ansciitable]
mov edi,0x4000
mov ecx,0x1000
rep movsb

call endofcpu




buffer:
dq "rax",0
dq "rcx",0
dq "rdx",0
dq "rbx",0
dq "rsp",0
dq "rbp",0
dq "rsi",0
dq "rdi",0

dq "r8",0
dq "r9",0
dq "r10",0
dq "r11",0
dq "r12",0
dq "r13",0
dq "r14",0
dq "r15",0

dq "cr0",0
dq "i'm dead",0
dq "cr2",0
dq "cr3",0
dq "cr4",0
dq "cr8",0
dq "i'm dead",0
dq "i'm dead",0

dq "dr0",0
dq "dr1",0
dq "dr2",0
dq "dr3",0
dq "killer"
killer:dq 0
dq "reason"
reason:dq 0
dq "dr6",0
dq "dr7",0

dq "[rsp]",0
dq "[rsp+8]",0
dq "[rsp+16]",0
dq "[rsp+24]",0
dq "[rsp+32]",0
dq "[rsp+40]",0
dq "[rsp+48]",0
dq "[rsp+56]",0

paddingofplacedata:
times 0x800-(paddingofplacedata-startofplacedata) db 0
