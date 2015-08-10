[ORG 0x7c00]              ;loaded here
[bits 16]

startofefistub:




;______________part1:MZ header________________
;[0,1]:"MZ"
	;db 'M'				;dec bp		;it does not matter
	;db 'Z'				;pop dx		;do not worry
;	jmp efistuberror




;[<3c]:关中断,设置ds=es=ss=0,设置栈开始于7c00
	xor ax, ax				; make it zero
	mov ds,ax
	mov es,ax
	mov ss,ax				; stack starts at seg 0
	mov sp, 0x7c00
	cli						; no interrupts
	cld




;读出前几扇区，来判断当前状况
	mov ax,0x0208			;2号功能，读取数量
	mov cx,0x0001			;[8,15]:磁道低8位，[7,6]:磁道高2位，剩余：起始扇区
	mov dx,0x0080			;磁头，驱动器号
	mov bx,0x1000			;es:bx=目的地
	int 0x13
	jc efistuberror




;比较开头0x200字节是不是一样
	mov si,0x7c00
	mov di,0x1000
	mov cx,0x200
	repe cmpsb
	jne endofefistub			;不一样说明是被加载器加载的，往后跳就行
	jmp 0x0:0x1200				;一样就要做一些工作了




;[0x3c,0x3f]:填0补满0x3c个字节，后面4个字节是pe头的地址
	times 0x3c-($-$$)db 0
	dd 0x40
;--------------------------------------------




;_____________part2:PE header______________
;[0x40,]:"PE" header
	db 'P','E',0,0					;[+0,+3]:"PE"
	db 0x64,0x86					;[+4,+5]:x86_64
	dw 1							;[+6,+7]:多少节
	dd 0							;[+8,+b]:时间




	dd 0							;[+c,+f]:pointer to symbol table
	dd 0							;[+10,+13]:number of symbols




	dw optionend-optionalheader		;[+14,+15]:size of optional header
	dw 0x206						;[+16,+17]:		;Characteristics
					;IMAGE_FILE_DEBUG_STRIPPED |
					;IMAGE_FILE_EXECUTABLE_IMAGE |
					;IMAGE_FILE_LINE_NUMS_STRIPPED
;__________________________________________




;______________part3-1:optional header________________
optionalheader:
	db 0xb,0x2						;[+18,+19]:pe32+ format
	db 0x2,0x14						;[+1a,+1b]:linker version high,low




	dd 0;		textend-textstart			;[+1c,+1f]:size of .text
	dd 0;		dataend-datastart			;[+20,+23]:size of .data
	dd 0;		bssend-bssstart				;[+24,+27]:size of .bss




	dd 0		;textstart					;[+28,+2b]:address of entrypoint
	dd 0							;[+2c,+2f]:base of .text
optionend:
;____________________________________________




;________________part3-2:extra header_________________
extraheader:
	dq 0x6f540000				;[+30,+37]:imagebase
	dd 0						;[+38,+3b]:section alignment
	dd 0x20						;[+3c,+3f]:file alignment




	dw 0						;[+40,+41]:major operation system version
	dw 0						;[+42,+43]:minor operation system version
	dw 0						;[+44,+45]:major image version
	dw 0						;[+46,+47]:minor image version
	dw 0						;[+48,+49]:major subsystem version
	dw 0						;[+4a,+4b]:minor subsystem version
	dd 0						;[+4c,+4f]:win32 version value




	dd 0xe000					;[+50,+53]:size of image
	dd 0x200					;[+54,+57]:size of headers
	dd 0						;[+58,+5b]:checksum
	dw 0xa						;[+5c,+5d]:subsystem(efi application)
	dw 0						;[+5e,+5f]:dllcharacteristics




	dq 0						;[+60,+67]size of stack reserve
	dq 0						;[+68,+6f]size of stack commit
	dq 0						;[+70,+77]size of heap reserve
	dq 0						;[+78,+7f]size of heap commit




	dd 0						;[+80,+83]:loader flags
	dd 0x6						;[+84,+87]:number of rvaandsizes




	dq 0						;[+88,+8f]:export table
	dq 0						;[+90,+97]:import table
	dq 0						;[+98,+9f]:resource table
	dq 0						;[+a0,+a7]:exception table
	dq 0						;[+a8,+af]:certification table
	dq 0						;[+b0,+b7]:base relocation table
;_______________________________________________




;________________part3:section table___________________
sectiontable:

	dq ".reloc"					;[+e0,+107]
	dd 0
	dd 0
	dd 0						;size of raw data

	dd 0						;pointer to raw data
	dd 0						;pointer to relocations
	dd 0						;pointer to linenumbers

	dw 0						;number of relocations
	dw 0						;number of linenumbers

	db 0x40,0x0,0x10,0x42		;section flag of .reloc




	dq ".text"					;[+108,+12f]

	dd 0
	dd 0
	dd 0

	dd 0
	dd 0
	dd 0

	dw 0
	dw 0

	db 0x20,0x0,0x50,0x60		;section flag of .text(same as .setup)




	dq ".bss"					;[+130,+158]

	dd 0
	dd 0
	dd 0						;size of initialized data

	dd 0
	dd 0						;pointer to relocations
	dd 0						;pointer to linenumbers

	dw 0						;number of relocations
	dw 0						;number of linenumbers

	db 0x80,0x0,0x00,0xc8		;section flag of .bss
;_________________________________________________




;_______________________________________________
efistubprintnibbie:
	and al,0xf
	add al,0x30
	cmp al,0x3a
	jb .return
	add al,7
.return:
	mov ah,0xf0
	stosw
	ret

efistuberror:
	mov bx,0xb800
	mov es,bx
	mov di,4*2

	mov al,ah
	mov bl,al
	shr al,4
	call efistubprintnibbie
	mov al,bl
	call efistubprintnibbie

efistubdie:
	hlt
	jmp efistubdie
;_________________________________________________




times 446-($-$$)db 0




;_________________________________________________
;first partition
	db 0x80,0xfe,0xff,0xff
	db 0x0b,0xfe,0xff,0xff
	dd 0x8		;假的首扇区号，0x200*8=0x1000=4k
	dd 0x78		;假的扇区数量(太小grub4dos不认)，0x200*(0x80-8)=0xf000=60k

;second partition
	dd 0
	dd 0
	dd 0
	dd 0

;third partition
	dd 0
	dd 0
	dd 0
	dd 0

;fourth partition
	dd 0
	dd 0
	dd 0
	dd 0

;填空
	times 510-($-$$)db 0
;signature
	dw 0xaa55			;假的磁盘可用标志
;___________________________________________________

endofefistub: