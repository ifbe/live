%define screeninfo 0x1000
%define ansciihome 0x30000
[bits 64]




;_________________________________
character:
    push rcx

    movzx eax,al
    shl eax,9
    lea esi,[eax+ansciihome]

    xor ecx,ecx
.lines:		;每排8个DWORD=4个QWORD
    lodsq
    mov [edi+ecx],rax
    lodsq
    mov [edi+ecx+8],rax
    lodsq
    mov [edi+ecx+16],rax
    lodsq
    mov [edi+ecx+24],rax

    add ecx,1024*4
    cmp ecx,1024*4*16
    jna .lines

.return:
    add edi,32
    pop rcx
    ret
;____________________________________




;__________________________________
char:
	cmp al,0x80
	jae .blank
	cmp al,0x20
	jb .blank
	call character
	ret

	.blank:
	mov al,0x20
	call character
	ret
;____________________________________




;把一串字符直接显示到edi指定的屏幕位置
;_______________________________
message:
	mov ecx,16
.continue:
	push rcx
	push rsi
	lodsb
	cmp al,0x20
	ja .thisdone
	add edi,32
	jmp .notprint
.thisdone:
	call char
.notprint:
	pop rsi
	pop rcx
	inc esi
	loop .continue
	ret
;______________________________




;把rbx的值直接显示到edi指定的屏幕位置
;_________________________________
dumprbx:
	mov ecx,16
	mov byte [rel firstnonzero],0
.getaddress:
        rol rbx,4
        mov al,bl
        and al,0x0f

	cmp byte [rel firstnonzero],0		;have first non zero?
	ja .print
	cmp ecx,1				;last value?
	je .print

	cmp al,0				;not print 0
	je .notprint

	.setsignal:				;now we have a non zero
	mov byte [rel firstnonzero],1

	.print:
        push rbx
        push rcx
        call character
        pop rcx
        pop rbx
	loop .getaddress

	ret

	.notprint:
	add edi,32
	loop .getaddress

	ret
;________________________________
firstnonzero: db 0




;_____________________________________________
writescreen0:
    mov esi,0x1000000				;[16m,20m)
    mov edi,[screeninfo+0x28]
    mov bl,[screeninfo+0x19]
    shr bl,3
    movzx ebx,bl
    mov ecx,1024*768
.continue:
    lodsd
    mov [edi],eax
    add edi,ebx
    loop .continue

	ret
;_____________________________________________




;_________________________________________
writescreen2:
	mov esi,[rel jpegbase]
	mov edi,[screeninfo+0x28]
	mov bl,[screeninfo+0x19]
	shr bl,3
	movzx ebx,bl
	mov ecx,1024*768
.continuescreen:
	lodsd
	mov [edi],eax
	add edi,ebx
	loop .continuescreen

	ret
;_________________________________________
jpegbase:dq 0x1400000




;_____________________________________________
writescreen3:
	mov esi,0x1800000
	mov edi,[screeninfo+0x28]
	mov bl,[screeninfo+0x19]
	shr bl,3
	movzx ebx,bl
	mov ecx,1024*768
.continue:
	lodsd
	mov [edi],eax
	add edi,ebx
	loop .continue

	ret
;______________________________________________




;_____________________________________________
writescreen4:
	mov esi,0x1c00000
	mov edi,[screeninfo+0x28]
	mov bl,[screeninfo+0x19]
	shr bl,3
	movzx ebx,bl
	mov ecx,1024*768
.continuescreen:
	lodsd
	mov [edi],eax
	add edi,ebx
	loop .continuescreen

	ret
;________________________________________________