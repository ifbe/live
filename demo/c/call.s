org 0x10000
[bits 64]
start:
mov rsp,0x1000000    ;16m
mov esi,0x20000
mov edi,0x100000
mov ecx,0x2000
rep movsq

;__________________________________
blackmagic:
	mov esi,address
	mov ecx,16
	continue:
		lodsb
		sub al,0x30
		cmp al,0x10
		jb next
		sub al,0x27
	next:
		shl rdx,4
		add dl,al
		loop continue

	add rdx,0x100000
;_______________________________________


;int3       ;debug
call rdx

death:hlt
jmp death

end:
times 0x100-(end-start) db 0

address:
