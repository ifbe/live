[bits 64]




;____________________________________________
f2init:
	ret
;___________________________________________




;_________________picture_______________________
f2show:
	mov rbp,[rel rgbabase]
	jmp writescreen
;_________________________________________
rgbabase:dq 0x1400000			;16m+4m








;_________________________________________
f2event:

.left:
	cmp al,0x4b
	jne .notleft

	cmp qword [rel rgbabase],0x400000
	jb .return
	sub qword [rel rgbabase],0x400000
	ret
.notleft:

.right:
	cmp al,0x4d
	jne .notright

	add qword [rel rgbabase],0x400000
	ret
.notright:

.up:
	cmp al,0x48
	jne .notup

	cmp qword [rel rgbabase],0x10000
	jb .return
	sub qword [rel rgbabase],0x10000
	ret
.notup:

.down:
	cmp al,0x50
	jne .notdown

	add qword [rel rgbabase],0x10000
	ret
.notdown:

.return:
	ret
;________________________________________
