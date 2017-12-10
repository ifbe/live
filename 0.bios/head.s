%define birthrecord 0x600
[ORG 0x7c00]              ; add to offsets
startofmbr:




;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;关中断,设置ds=es=ss=0,设置栈开始于7c00
	mov ax,0xb800
	mov es,ax
	xor di,di
	mov ax,0xf030
	stosw					;打印“0”，说明mbr跑起来了

	xor ax, ax				;make it zero
	mov ds,ax
	mov es,ax
	mov ss,ax				;stack starts at seg 0
	mov sp, 0x7c00
	cld
	cli
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>




;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;读出前几扇区到0x1000，来判断当前状况
	mov ax,0x0208			;2号功能，读取数量
	mov cx,0x0001			;[8,15]:磁道低8位，[7,6]:磁道高2位，剩余：起始扇区
	mov dx,0x0080			;磁头，驱动器号
	mov bx,0x1000			;es:bx=目的地
	int 0x13
	jc mbrerror

;比较开头0x200字节是不是一样
	mov si,0x7c00
	mov di,0x1000
	mov cx,0x200
	repe cmpsb
	je readyourself			;一样说明没用到引导器，要自己读64k进内存
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>




;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;不一样说明代码是被引导器加载的，但是不知具体位置所以自己搬
	xor eax,eax
	xor ebx,ebx

	mov ax,cs
	shl eax,4			;拿到cs<<4
	call whereami
whereami:
	pop bx				;拿到ip
	add eax,ebx
	sub eax,whereami-startofmbr

;拿到实际位置
	mov [birthrecord],eax

	shr eax,4
	mov ds,ax			;ds
	mov ax,0x1000
	mov es,ax			;es
	mov cx,0x8000			;cx=(0x10000-0x200)/2

	cmp word [birthrecord+2],0
	je reversemove
		mov si,0		;si
		mov di,si		;di
		cld
		rep movsw
		jmp mbrsuccess

	reversemove:
		mov si,0xfffe
		mov di,si
		std
		rep movsw
		jmp mbrsuccess
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>




;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;全部一样说明在虚拟机里，读前128个扇区到0x10000然后跳到+0x200的位置
readyourself:				;dap:
	mov byte [0xff0],0x10		;.packagesize db 0x10
	mov byte [0xff1],0		;.reserved db 0x00
	mov word [0xff2],0x40		;.blockcount dw 0x0002
	mov word [0xff4],0		;.bufferoffset dw 0xc000
	mov word [0xff6],0x1000		;.buffersegment dw 0x0000
	mov dword [0xff8],0		;.blocknumber dq 0x0000000000000001
	mov dword [0xffc],0

	mov ah,0x42
	mov dl,0x80
	mov si,0xff0
	int 0x13
	jc mbrerror

	mov byte [0xff0],0x10		;.packagesize db 0x10
	mov byte [0xff1],0		;.reserved db 0x00
	mov word [0xff2],0x40		;.blockcount dw 0x0002
	mov word [0xff4],0x8000		;.bufferoffset dw 0xc000
	mov word [0xff6],0x1000		;.buffersegment dw 0x0000
	mov dword [0xff8],0x40		;.blocknumber dq 0x0000000000000001
	mov dword [0xffc],0

	mov ah,0x42
	mov dl,0x80
	mov si,0xff0
	int 0x13
	jc mbrerror

	jmp mbrsuccess
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.




;_______________________________________________
mbrprintnibbie:
	and al,0xf
	add al,0x30
	cmp al,0x3a
	jb .return
	add al,7
.return:
	mov ah,0xf0
	stosw
	ret

mbrerror:
	mov bx,0xb800
	mov es,bx
	mov di,4*2

	mov al,ah
	mov bl,al
	shr al,4
	call mbrprintnibbie
	mov al,bl
	call mbrprintnibbie

mbrdie:
	hlt
	jmp mbrdie

mbrsuccess:
	jmp 0x1000:0x4000
;_________________________________________________




times 0x1be-($-$$)db 0




;_____________________________________________
db 0x80,0xfe,0xff,0xff
db 0x0b,0xfe,0xff,0xff
dd 0x800		;首扇区号
dd 0x1f000		;扇区数量

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

db 0x55,0xaa
;_____________________________________________




endofmbr:
