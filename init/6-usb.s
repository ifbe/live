startofusb:
[BITS 64]
;________________________________
usb:
    mov esi,0x900a
.searchusb:
    mov ax,[esi]
    cmp ax,0x0c03
    jne .next
    call findusb
    and al,0xf0
    stosq
    mov dword eax,"?hci"
    cmp byte [esi-1],0x30
    jne .noxhci
    mov al,'x'
  .noxhci:
    cmp byte [esi-1],0x20
    jne .noehci
    mov al,'e'
  .noehci:
    stosq
.next:
    add esi,0x10
    cmp esi,0xa000
    jb .searchusb
    jmp usbfinish
;________________________________________


;_______________________________________
findusb:
    cmp byte [0x9003],0x80
    jne usbnew

usbold:
    mov eax,[esi-0xa]
    add eax,0x10
    mov dx,0xcf8
    out dx,eax
    mov dx,0xcfc
    in eax,dx
    ret

usbnew:
    mov eax,[esi-0xa]
    add eax,0x10             ;BAR0
    mov eax,[eax]
    ret
;_____________________________________


usbfinish:
