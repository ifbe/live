[ORG 0x7c00]              ; add to offsets
startofmbr:




;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;关中断,设置ds=es=ss=0,设置栈开始于7c00
	cli                    ; no interrupts
	xor ax, ax             ; make it zero
	mov ds,ax
	mov es,ax
	mov ss,ax             ; stack starts at seg 0
	mov sp, 0x7c00

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>




;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;先读首扇区到一个临时的位置:0x800
	mov dword [dap.blocknumber],0
	mov byte [dap.blockcount],1
	mov word [dap.buffersegment],0
	mov word [dap.bufferoffset],0x800
	call loadsector
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>




;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;跟这个文件前256个字节(后面256个会改变)不一样的话，就跳到本文件结尾
	mov esi,0x7c00
	mov edi,0x800
	mov ecx,0x100
	cld
	rep cmpsb
	je loadinit
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>




;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;不一样说明代码是被引导器加载的，但是不知具体位置所以比必须自己搬家
	xor eax,eax
	xor ebx,ebx

	call whereami
whereami:
	pop bx				;拿到ip

	mov ax,cs
	shl eax,4			;拿到cs<<4

	add eax,ebx
	sub eax,whereami-startofmbr
	mov [0x4e0],eax		;拿到本程序被加载到的实际位置

	shr eax,4
	mov ds,ax			;ds
	mov ax,0x1000
	mov es,ax			;es
	mov cx,0x7f00		;cx=(0x10000-0x200)/2

	cmp word [0x4e2],0
	jb reversemove
		mov si,0x200			;si
		mov di,si			;di
		cld
		rep movsw
		jmp 0x1000:0x0200

	reversemove:
		mov si,0xfffe
		mov di,si
		std
		rep movsw
		jmp 0x1000:0x0200
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>




;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;全部一样说明在虚拟机里，读前128个扇区到0x10000然后跳到+0x200的位置
loadinit:
	xor eax,eax
	mov bx,0x1000
	;mov cx,2            ;[10000,8ffff]
;continue:
	mov dword [dap.blocknumber],eax
	mov byte [dap.blockcount],64
	mov word [dap.buffersegment],bx
	mov word [dap.bufferoffset],0x0000
	call loadsector
	add eax,64

	mov dword [dap.blocknumber],eax
	mov byte [dap.blockcount],64
	mov word [dap.buffersegment],bx
	mov word [dap.bufferoffset],0x8000
	call loadsector
	add eax,64

	;add bx,0x1000
	;loop continue

	jmp 0x1000:0x0200
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



times 384-($-$$)db 0


;<<<<<<<<<<<<<<load sector<<<<<<<<<<<<<<<<<<<<<<<
loadsector:
	pushad
	mov ah,0x42
	mov dl,0x80
	mov si,dap
	int 0x13
	popad
	ret
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>




;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
dap:
	.packagesize db 0x10
	.reserved db 0x00
	.blockcount dw 0x0002
	.bufferoffset dw 0xc000
	.buffersegment dw 0x0000
	.blocknumber dq 0x0000000000000001
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.


times 446-($-$$)db 0




db 0x80,0xfe,0xff,0xff
db 0x0b,0xfe,0xff,0xff
dd 0x800		;首扇区号
dd 0x1f800		;扇区数量


dd 0
dd 0
dd 0
dd 0


dd 0
dd 0
dd 0
dd 0


dd 0
dd 0
dd 0
dd 0


dw 0xaa55

endofmbr:
