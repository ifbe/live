bits 64

cld
mov rcx,0x20000
xor rax,rax
mov edi,0x100000
rep stosq

mov edi,0x100000
mov rax,"/"
stosq
mov rax,0x100000
stosq

mov rax,"/anscii"
stosq
mov rax,0x110000
stosq

mov rax,"/console"
stosq
mov rax,0x120000
stosq

mov rax,"/journal"
stosq
mov rax,0x130000
stosq

mov rax,"/pci"
stosq
mov rax,0x140000
stosq

mov rax,"/usb"
stosq
mov rax,0x150000
stosq

mov rax,"/acpi"
stosq
mov rax,0x160000
stosq

mov rax,"/bin"
stosq
mov rax,0x180000
stosq

mov rax,"/todo"
stosq
mov rax,0x190000
stosq
