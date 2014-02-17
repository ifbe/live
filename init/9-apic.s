[bits 64]
cli
mov edi,0x8000
searchlocalapic:         ;(apic)?
cmp dword [edi],0
je localapic             ;yes(use apic)
add edi,0x10
cmp edi,0x9000
jb searchlocalapic

noapic:hlt                 ;no,die
jmp noapic


localapic:
mov rcx,0x1b
rdmsr
;bts eax,11            ;enable lapic
;wrmsr
and eax,0xfffff000    ;save lapic base
stosd
mov [edi],edx
add edi,0x4
mov rax,"LAPIC   "
stosq
