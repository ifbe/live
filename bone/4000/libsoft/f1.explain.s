[bits 64]




;___________________________________
f1explain:
	lea r8,[rel input]
	call buf2arg

.debug:
	;mov rax,[rel arg0]
	;mov [rel message0+0x80],rax
	;mov qword [rel message0+0xc0],"haha"

.start:
	lea esi,[rel arg0]

	cmp dword [esi],"exit"
	je poweroff

	cmp dword [esi],"powe"		;if( ( [esi]=="powe" )
	jne .skippoweroff
	cmp dword [esi+4],"roff"	;&& ( [esi+4]=="roff" ) )
	je poweroff
.skippoweroff:

	cmp dword [esi],"addr"
	je changeaddress

	cmp dword [esi],"chan"
	jne .skipchange
	cmp dword [esi+4],"ge"
	je changememory
.skipchange:

	cmp dword [esi],"sear"
	jne .skipsearch
	cmp dword [esi+4],"ch"
	je searchinthisscreen
.skipsearch:

.return:
	ret
;____________________________________