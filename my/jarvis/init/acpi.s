;/acpi
;如果要改位置,搜索0x100000和0x101000


[BITS 64]


startofacpi:
;______________________clear____________________________
mov edi,0x100000
mov ecx,0x2000
xor rax,rax
rep stosq
;____________________________________________________




;_________look for rsd ptr from e0000~ffff0__________
    mov rax,"RSD PTR "
    mov edi,0x100000
    stosq
    mov rbp,0xe0000
    mov ecx,0x2000
rsdptr:
    cmp [rbp],rax
    je findrsdptr		;找到的地址在rbp
    add rbp,0x10
    loop rsdptr

    jmp endofacpi
;_____________________________________________




;________________rsdt/xsdt?_________________
findrsdptr:
    mov rax,rbp       ;rsdptr
    stosq
    cmp byte [rbp+0xf],0x00 ;不是0就是xsdt
    jne xsdt
;_________________________________________




;____________________________________
rsdt:
    mov eax,"RSDT"
    stosq
    mov eax,[rbp+0x10]
    stosq
    mov cl,[eax+4]
    sub cl,0x24
    add eax,0x24
.rsdttable:			;把每个表翻译到指定地方（di）
    mov esi,[eax]
    mov [edi+8],esi		;地址
    movsd			;名字
    add edi,0xc
    add eax,0x4
    sub cl,4
    cmp cl,0
    ja .rsdttable

    jmp acpi
;__________________________________




;____________________________________
xsdt:
    mov eax,"XSDT"
    stosq
    mov rax,[rbp+0x18]
    stosq
    mov cl,[rax+4]
    sub cl,0x24
    add eax,0x24
.xsdttable:			;把每个表翻译到指定地方（di）
    mov esi,[eax]
    mov [edi+0x8],esi
    movsq
    add edi,0x8
    add eax,0x8
    sub cl,8
    cmp cl,0
    ja .xsdttable
;_____________________________________________




;_______________处理每一项______________________
acpi:
    mov dword [rel saveaddr],0x100000

.search:
    mov esi,[rel saveaddr]	;switch(名字)

    cmp dword [esi],"FACP"	;case facp
    jne .notfacp
    call explainfacp
    .notfacp:

    cmp dword [esi],"APIC"	;case apic
    jne .notapic
    call explainapic
    .notapic:

    cmp dword [esi],"MADT"	;case madt
    jne .notmadt
    call explainapic
    .notmadt:

    cmp dword [esi],"what"	;case else....
    jne .notwhat
    call explainelse
    .notwhat:

    add dword [rel saveaddr],0x10
    cmp dword [rel saveaddr],0x101000
    jb .search

    jmp endofacpi
;_________________________________________________
saveaddr:dq 0




;[esi]:名字
;[esi+8]:地址
;edi:往哪儿写
;_________________________________________________
explainfacp:
    mov esi,[esi+0x8]           ;facpaddr=[esi-0x8]

    mov eax,[esi+0x28]		;dsdtaddr=[facpaddr+0x28]
    mov [edi+8],eax
    mov eax,[eax]		;dsdt=[dsdtaddr]
    mov [edi],eax
    add edi,0x10

    mov edx,[edi-8]		;dsdtaddr
    mov eax,"_S5_"
    stosq
searchs5:			;dsdt
    cmp [edx],eax
    jne .nots5
	mov eax,edx
	stosq
	jmp finds5
    .nots5:
    inc edx
    jmp searchs5		;we must find it
    ;cmp edx,0x100000
    ;jb searchs5
    ;ret

finds5:
    mov eax,"data"
    stosq
    xor rax,rax
    mov al,[edx+8]		;edx=_s5_addr
    shl ax,10
    stosq
    mov [0x4fe],ax

    mov eax,"port"
    stosq
    xor rax,rax
    mov ax,[esi+0x40]           ;data=[facpaddr+0x40]
    stosq
    mov [0x4fc],ax

    ret


    ;how to shutdown:
    ;mov dx,port
    ;mov ax,data
    ;or ax,0x2000
    ;out dx,ax
;_________________________________________




;[esi]:名字
;[esi+8]:地址
;edi:往哪儿写
;_________________________________
explainapic:
    mov esi,[esi+8]			;madtaddr

    mov rax,"table"
    stosq
    lea eax,[esi+0x2c]
    stosq

    mov edx,eax				;madtstart
    mov ecx,[esi+4]
    add ecx,esi				;madtend
analyse:
    cmp byte [edx],1
    jne .continue

    mov rax,"ioapic"
    stosq
    mov eax,[edx+4]
    stosq
    ret

.continue:
    mov al,[edx+1]
    movzx eax,al
    add edx,eax
    cmp edx,ecx
    jb analyse

    ret
;_____________________________




explainelse:
ret


endofacpi:




;[0,3]:"$PIR"
;[4,5]:version
;[6,7]:tablesize
;[8]:pci interrupt router's bus
;[9]:pci interrupt router's devfunc
;[0xa,0xb]:pci exclusive irqs
;[0xc,0xf]:compatible pci interrupt router
;[0x10,0x13]:miniport data
;[0x14,1e]:0
;[0x1f]:checksum
;[0x20+0x10*slot,0x2f+0x10*slot]:pci slot entry

;[0]:pci bus number
;[1]:pci device number(upper 5bit)
;[2]:link value for inta#
;[3,0x4]:irq bitmap for inta#
;[0x5]:link value for intb#
;[0x6,0x7]:irq bitmap for intb#
;[8]:link value for intc#
;[9,0xa]:irq bitmap for intc#
;[0xb]:link value for intd#
;[0xc,0xd]:irq bitmap for intd#
;[0xe]:slot number
;[0xf]:reserved

pciirqrouting:
    ;上面的rdi没变
    mov eax,"$PIR"
    mov rbp,0xe0000
    mov ecx,0x2000
.searchtable:
    cmp [rbp],eax
    je .findtable
    add rbp,0x10
    loop .searchtable

    jmp routingtabledone

.findtable:
    stosq
    mov [rdi],rbp
    add rdi,8


routingtabledone:
