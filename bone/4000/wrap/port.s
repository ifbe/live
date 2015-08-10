bits 64


;__________________________
poweroff:
mov dx,[0x4fc]
mov ax,[0x4fe]
or ax,0x2000
out dx,ax
;__________________________




;__________________________________
reboot:
mov al,0xfe
out 0x64,al
;____________________________________




;入：r8,r9,r10,r11......
;出：r8,r9,r10,r11......
;_____________________________________
gettime:
	xor rax,rax				;must clean

	mov al,0x32				;[63:56]:centry
	out 0x70,al
	in al,0x71

	shl rax,8
	mov al,9				;[55,48]:year
	out 0x70,al
	in al,0x71

	shl rax,8
	mov al,8				;[47,40]:month
	out 0x70,al
	in al,0x71

	shl rax,8
	mov al,7				;[39,32]:day
	out 0x70,al
	in al,0x71

	shl rax,8
	mov al,4				;[31,24]:hour
	out 0x70,al
	in al,0x71

	shl rax,8
	mov al,2				;[23,16]:minute
	out 0x70,al
	in al,0x71

	shl rax,8
	mov al,0				;[15,8]:second
	out 0x70,al
	in al,0x71
	shl rax,8

	mov r8,rax
	call data2string

	mov word [rel string+0xe],0x2020		;milesecond
	mov rax,[rel string]
	mov [rel time],rax
	mov rax,[rel string+8]
	mov [rel time+8],rax

	ret
;___________________________________________