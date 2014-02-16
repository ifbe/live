%include "../s/readmemory.s"
[BITS 64]
    mov eax,0x80000000                 ;device: bus#=0,dev#=1,func#=1

    mov dx,0xcf8     ;addr register port
    out dx,eax       ;Find pci io configuration register
    mov dx,0xcfc     ;read data from data register port
    in eax,dx
    ret
