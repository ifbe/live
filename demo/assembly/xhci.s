startofusb:
[BITS 64]
;________________________________
emptyforusb:
mov edi,0x5000

.continue:
	cmp dword [edi+8],0
	je usb
	cmp dword [edi+8],"xhci"
	je usb
	cmp dword [edi+8],"ehci"
	je usb
	cmp dword [edi+8],"?hci"
	je usb

add edi,0x10
cmp edi,0x6000
jb .continue
jmp endofusb


usb:
    mov esi,0x500a
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
    cmp esi,0x6000
    jb .searchusb
    jmp endofusb
;________________________________________


;_______________________________________
findusb:
    cmp byte [0x5003],0x80
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

paddingofusb:
times 0x7ff-(paddingofusb-startofusb) db 0

endofusb:
ret
