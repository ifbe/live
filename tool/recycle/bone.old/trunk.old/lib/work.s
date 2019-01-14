bits 64
;_________________________
f1:
	lea rax,[rel function1]
	mov [rel screenwhat],rax
	jmp ramdump
f2:
	lea rax,[rel function2]
	mov [rel screenwhat],rax
	jmp picture
f3:
	lea rax,[rel function3]
	mov [rel screenwhat],rax
	jmp cyberspace
f4:
	lea rax,[rel function4]
	mov [rel screenwhat],rax
	jmp console
f5:
	lea rax,[rel function5]
	mov [rel screenwhat],rax
	jmp fallback
;___________________________________________
