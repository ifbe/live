;/pci
%define pcihome 0x140000


[BITS 64]


;explainpci
;[0,7]:(vendorid<<16)+deviceid
;[8,0xf]:(class<<24)+(subclass<<16)+(progif<<8)+revisionid
;[0x10,0x17]:portaddress of the device
;[0x18,0x1f]:ansciiname of the device
startofpci:
	mov edi,pcihome
	mov ecx,0x2000
	xor rax,rax
	rep stosq

	mov edi,pcihome
	mov ecx,0xffff
	mov ebx,0x80000000
.enumeration:
	mov eax,ebx
	mov dx,0xcf8
	out dx,eax
	mov dx,0xcfc
	in eax,dx
	cmp eax,0xffffffff
	je .empty

	mov eax,ebx
        mov dx,0xcf8       ;id
        out dx,eax
        mov dx,0xcfc
        in eax,dx
	mov [edi],eax

        lea eax,[ebx+8]    ;class
        mov dx,0xcf8
        out dx,eax
        mov dx,0xcfc
        in eax,dx
	mov [edi+8],eax

	mov [edi+0x10],ebx
	add edi,0x40
.empty:
	add ebx,0x100
	loop .enumeration
;________________________________________


endofpci:
