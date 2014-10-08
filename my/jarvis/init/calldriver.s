bits 64


mov ecx,0x8000
mov edi,0x40000
xor rax,rax
rep stosq

mov ecx,0x10000
mov edi,0x180000
xor rax,rax
rep stosq
;_______________________________________
				;cpu@0x0
				;jarvis@0x2000

;initahci
call endofjarvis		;ahci@0x4000

;initxhci
call endofjarvis+0x2000		;xhci@0x6000

;initusb
call endofjarvis+0x4000		;usb@0x8000

;initdisk
call endofjarvis+0x8000		;disk@0xc000
;__________________________________
