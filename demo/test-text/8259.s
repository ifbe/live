;init/[1,7]

;___________________8259_____________________
;前面已经out *,0xff
icw1:
    mov al,0x11
    out 0x20,al
    out 0xa0,al
icw2:
    mov al,0x20
    out 0x21,al
    mov al,0x28
    out 0xa1,al
icw3:
    mov al,0x4
    out 0x21,al
    mov al,0x2
    out 0xa1,al
icw4:
    mov al,0x01                ;03=aeoi
    out 0x21,al
    out 0xa1,al

    mov al,0xf9            ;enable
    out 0x21,al
    mov al,0xfe
    out 0xa1,al
;__________________________________________



;__________________int3h______________________
    mov rax,breakpointisr
    mov edi,0x1030
    call idtinstall
;______________________________________________


;_____________int9h>>int21h__________________
irq1:
    mov rax,keyboardisr
    mov edi,0x1210
    call idtinstall
;________________________________________


;___________int70h>>int28h_________________
irq8:
    mov al,0x8a
    out 0x70,al
    mov al,0xaf           ;3=8192hz,6=1024hz,f=1hz
    out 0x71,al

    mov al,0x8b
    out 0x70,al
    in al,0x71

    mov ah,al
    or ah,0x40

    mov al,0x8b
    out 0x70,al
    mov al,ah
    out 0x71,al

    mov rax,rtcisr
    mov edi,0x1280
    call idtinstall
;_________________________________________


;__________________idt____________________
    mov word [idtptr.length],0xfff
    mov qword [idtptr.base],0x1000
    lidt [idtptr]
;_______________________________________


;__________________jump_____________________
    sti
    mov dword [0xff8],0xb8000
    jmp idtdone
;_________________________________________


;___________________________________
idtinstall:
    stosw
    mov dword [edi],0x8e000008
    add edi,4
    shr rax,16
    stosw
    shr rax,16
    stosd
    xor eax,eax
    stosd
    ret
;____________________________________


;_____________________________________
ALIGN 16
idtptr:
.length dw 0
.base dq 0
;_____________________________________


;_____________________________________
breakpointisr:
    mov dword [0xb8400],0x12431243          ;test
    iretq
;_____________________________________


;_____________________________________
rtcisr:
    push rax
    mov eax,[0xff8]
    mov word [eax],0x0730
    add dword [0xff8],2

    mov al,0x8c             ;must read ,or there is no interrupt
    out 0x70,al
    in al,0x71

    mov al,0x20
    out 0xa0,al
    out 0x20,al
    pop rax
    iretq
;_____________________________________


;_____________________________________
keyboardisr:
    push rax
    push rbx
    in al,0x60
    cmp al,0x80
    jb .start
    jmp .leave
.start:
    mov ebx,0xb800
.search:
    cmp [ebx],al
    je .find
    add ebx,2
    cmp ebx,0xb888
    jnb .finish
    jmp .search
.find:
    mov al,[ebx+1]
.finish:
    mov ah,0x70
    mov ebx,[0xff8]
    mov [ebx],ax
    add word [0xff8],2
.leave:
    mov al,0x20
    out 0x20,al
    pop rbx
    pop rax
    iretq
;________________________________________


idtdone:
    int3


death:hlt
jmp death
