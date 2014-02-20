startofdisk:
[BITS 64]
;________________________________
disk:
    mov esi,0x5008
searchdisk:
    mov eax,[esi]
    mov al,0
    cmp eax,0x01060100
    jne .next
    call finddisk
    and al,0xf0
    stosq

    mov dword eax,"ahci"
    stosq
.next:
    add esi,0x10
    cmp esi,0x6000
    jb searchdisk
    jmp endofdisk
;________________________________________


;_______________________________________
finddisk:
    cmp byte [0x5003],0x80
    jne disknew

diskold:
    mov eax,[esi-0x8]
    add eax,0x24
    mov dx,0xcf8
    out dx,eax
    mov dx,0xcfc
    in eax,dx
    ret

disknew:
    mov eax,[esi-0x8]
    add eax,0x24             ;BAR5
    mov eax,[eax]
    ret
;_____________________________________


endofdisk:
