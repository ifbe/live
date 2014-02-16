%include "../init/init768.s"
[BITS 64]
;_________________________________________
    mov r8b,[0xa019]        ;r8=bit/点
    shr r8b,3               ;r8=byte/点

    mov r9,r8
    shl r9,10              ;r9=1024*r8

    mov r10,r8
    shl r10,3              ;r10=8*r8

    mov r11,r8
    shl r11,14              ;r11=16*1024*r8

                            ;r12 keyboard pointer
                            ;r13 unused
                            ;r14 memory pointer
                            ;r15 offset pointer
;___________________________________________


;____________before main_________________
    mov qword [0x2000],0x0000000000000000
    mov r12,0x1000
    mov r14,0x8000
    xor r15,r15
;_____________________________


;_____________main____________________
forever:
    call displaymemory

keyboard:
    in      al,0x64 
    test    al,1
    jz keyboard

    in al,0x60
    cmp al,0x80
    ja keyboard                  ;按下显示，弹起不管

switchkey:
    cmp al,0x0e
    je backspace
    cmp al,0x01
    je turnoff
    cmp al,0x29
    je command
    cmp al,0x0f
    je tab
    cmp al,0x3a
    je capslock
    cmp al,0x2a
    je shift
    cmp al,0x1d
    je ctrl
    cmp al,0x5b
    je super
    cmp al,0x38
    je alt
    cmp al,0x1c
    je enter
    cmp al,0x49
    je pageup
    cmp al,0x51 
    je pagedown
    cmp al,0x47
    je home
    cmp al,0x4f 
    je end
    cmp al,0x4b
    je left
    cmp al,0x4d
    je right
    cmp al,0x48
    je up
    cmp al,0x50
    je down

    mov [r12],al
    inc r12
    cmp r12,0x2000
    jb forever

    call clearbuffer

    jmp forever

    DIE:hlt
    jmp DIE
;_________________________________


;___________________________________
turnoff:
    mov dx,[0x4fc]
    mov ax,[0x4fe]
    or ax,0x2000
    out dx,ax
backspace:
    dec r12
    mov byte [r12],0
    cmp r12,0x1000
    ja forever
    mov r12,0x1000
    jmp forever
esc:
    mov al,[0x2000]
    inc al
    and al,0x01
    mov [0x2000],al
    jmp forever
command:
    mov al,[0x2001]
    inc al
    and al,0x01
    mov [0x2001],al
    jmp forever
tab:
    mov al,[0x2002]
    inc al
    and al,0x01
    mov [0x2002],al
    jmp forever
capslock:
    mov al,[0x2003]
    inc al
    and al,0x01
    mov [0x2003],al
    jmp forever
shift:
    mov al,[0x2004]
    inc al
    and al,0x01
    mov [0x2004],al
    jmp forever
ctrl:
    mov al,[0x2005]
    inc al
    and al,0x01
    mov [0x2005],al
    jmp forever
super:
    mov al,[0x2006]
    inc al
    and al,0x01
    mov [0x2006],al
    jmp forever
alt:
    mov al,[0x2007]
    inc al
    and al,0x01
    mov [0x2007],al
    jmp forever
enter:
    call falsify                ;do something here
    call clearbuffer
    jmp forever
pageup:
    sub r14,0x800
    jmp forever
pagedown:
    add r14,0x800              ;a row
    jmp forever
home:
    sub r14,0x30
    jmp forever
end:
    add r14,0x30
    jmp forever
left:
    cmp r15,0
    jna forever
    dec r15
    jmp forever
right:
    cmp r15,0x86f
    jnb forever
    inc r15
    jmp forever
up:
    cmp r15,48
    jb forever
    sub r15,48
    jmp forever
down:
    cmp r15,0x83f
    ja forever
    add r15,48
    jmp forever
;________________________________


;_______al为高4位，ah为低4位，把ax翻译成16进制数字____________
;__________哈哈，1～9正常，f1～f6当成abcdef用!!______________
falsify:
    mov ax,[0x1000]
    cmp al,0
    jne oneortwo
    ret                         ;空缓冲区，return

oneortwo:
    cmp ah,0
    je falsifyhigh              ;只输入了一个，右边一个数不动

falsifylow:
    cmp ah,0x0b
    jne zhengchangah
    mov ah,1            ;不正常
    zhengchangah:
    dec ah
    and ah,0x0f
    mov bl,[r15+r14]
    and bl,0xf0                    ;左边一个数不动
    add bl,ah
    mov [r15+r14],bl

falsifyhigh:
    cmp al,0x0b
    jne zhengchangal
    mov al,1            ;不正常
    zhengchangal:
    cmp al,0x0a           ;>9就要一个个翻译
    dec al
    ;mov cl,4
    shl al,4
    mov bl,[r15+r14]      ;原始数据
    and bl,0x0f             ;高位清掉
    add bl,al               ;高位改掉
    mov [r15+r14],bl

    ret
;______________________________________________________


;__________________________________
clearbuffer:
    push rcx                   ;tmd不知道那儿要用到ch
    xor cx,cx
cb: mov qword [0x1000+rcx],0
    add cx,8
    cmp cx,0x1000
    jb cb
    mov r12,0x1000
    pop rcx
    ret
;_________________________________


;进:r14
;出:
;用:
;变:
;______________________________
displaymemory:
    mov edi,[0xa028]        ;vesabase
    mov eax,0xffff00
    mov ecx,1024*16
yellow:
    mov [edi],eax
    add edi,r8d
    loop yellow

    mov edi,[0xa028]
    mov cl,6
fivetimes:
    add edi,r10d
    loop fivetimes

    mov dl,0
    mov cl,6
sixtimes:
    lea edi,[8*r10d+edi]            ;edi=edi+16*r10
    lea edi,[8*r10d+edi]            ;leave one register alone
    call displayhex
    add dl,8
    loop sixtimes

    mov edi,[0xa028]
    add edi,r11d

    mov cl,45
    wolegeca:
    push rcx
        mov rdx,r14
        call displaypositive         ;地址

        mov cl,4
        bb: 
        mov esi,0xb200                    ;blank
        call char
        loop bb                      ;4空

        push rcx
            mov cl,6
            dp: 
            mov esi,0xb200                     ;blank
            call char
            call char                ;2空*6

            mov rdx,[r14]
            call displayinverted     ;16*6
            add r14,8
            loop dp                   ;16+4+18*6==1024/8
        pop rcx
    pop rcx
    loop wolegeca

    sub r14,0x870         ;恢复r14

    push rdi
    mov rdx,[0x2000]
    call displayinverted  ;显示键盘状态寄存器:esc,`,tab,cap,shift,ctrl,super,alt
    pop rdi

    add edi,r11d          ;挪到最后一行
    mov rdx,r15
    call displaypositive
    mov cl,6
kk:
    mov esi,0xb200                ;blank
    call char
    loop kk                      ;6空

    xor rcx,rcx
keyboardbuffer:
    mov dl,[ecx+0x1000]
    call displayanscii    ;显示键盘缓冲区
    inc rcx
    cmp cx,128
    jb keyboardbuffer

    mov si,0xb8b0                 ;kuang
    call pointer
    call utf8

    ret
;_____________________________________


;进:esi,eax
;出:esi
;用:
;变:cl,al
;_______________________________
number:
    and eax,0x000000ff
    shl ax,4
    mov esi,0xb000                     ;zero
    add esi,eax
    ret
;______________________________


;进:edi,r15d
;出:edi
;用:
;变:cl
;_______________________________
pointer:
    mov edi,[0xa028]
    add edi,r11d

    mov eax,r8d
    shl eax,4
    add edi,eax
    lea edi,[2*eax+edi]                ;跳过地址头
    lea edi,[8*eax+edi]

    mov eax,r15d
    mov bl,48               ;一排48个byte，计算在第几排
    div bl
    mov dl,ah
    xor ah,ah               ;清空之  

    mul r8d
    shl eax,14
    add edi,eax              ;哪一行

    mov eax,r15d
    and eax,0x00000007             ;mod8取余
shi:
    cmp al,0
    je jieshu8
    lea edi,[2*r10d+edi]
    dec al
    jmp shi
jieshu8:

    mov eax,r15d
    shr eax,3
    lea edx,[2*r10d]
mod:
    cmp ax,6
    jb sheng
    sub ax,6
    jmp mod
sheng:
    cmp al,0                     ;mod6取余
    je jieshu6
    lea edi,[8*edx+edi]
    add edi,edx
    dec al
    jmp sheng
jieshu6:

    ret
;_______________________________


;进:dl
;出:
;用:
;变:
;____________________________________________
displayhex:
    push rcx
    xor rax,rax
    mov al,dl
    shr al,4        ;high 4bit
    call number
    call char

    xor rax,rax
    mov al,dl
    and al,0x0f
    call number
    call char
    pop rcx
    ret
;__________________________________________


;进:dl
;出:
;用:
;变:
;____________________________________________
displayanscii:
    push rcx
    xor ecx,ecx
    mov cl,44
    mov esi,0xb800                 ;table
.beforeconvert:
    cmp [esi],dl            ;先把dl里的扫描码转换成anscii给al
    je .convert
    add esi,2
    loop .beforeconvert
    mov esi,0xb200           ;blank
    jmp goodbye
.convert:
    inc esi
    lodsb
    call number

goodbye:
    call char
    pop rcx
    ret
    
;__________________________________________


;进:rdx,edi
;出:
;用:
;变:
;_______________________________________________
displaypositive:
    push rcx
    xor rcx,rcx
    mov cl,16
manytimes:
    push rcx
        xor rax,rax
        rol rdx,4
        mov al,dl
        and al,0x0f
        call number
        call char
    pop rcx
    loop manytimes

    pop rcx
    ret
;__________________________________________________



;进:rdx,rdi
;出:
;用:
;变:
;____________________________________________
displayinverted:
    push rcx
    xor rcx,rcx
    mov cl,8
aoa:
    push rcx
        call displayhex
        mov cl,8
        ror rdx,cl
    pop rcx
    loop aoa

    pop rcx
    ret
;_________________________________________


;进:esi,edi
;出:esi,edi
;用:
;变:
;______________________________
char:
    push rbx
    push rcx
    xor rcx,rcx
    mov cl,16           ;16行
.yihang:
    xor rax,rax
    lodsb
    push rcx
        mov cl,8
      .yidian:
        mov bl,al
        and bl,0x80
        cmp bl,0x80
        jne .background
        mov dword [edi],0x00000000
        jmp .pointdone
      .background:
        mov dword [edi],0x00ffff00
      .pointdone:
        add edi,r8d
        shl al,1
        loop .yidian
    sub edi,r10d           ;每个字的行头
    add edi,r9d            ;下1行
    pop rcx
    loop .yihang

    add edi,r10d            ;下个字的行头
    sub edi,r11d            ;上16行;现在edi=下个字开头

    mov eax,edi
    mov ebx,[0xa028]        ;ebx=vesabase
    sub eax,ebx             ;eax=相对距离

    cmp r8,4
    je .modfour                  ;r8==4
    and eax,0x000003ff          ;r8==3则mod 1024
    cmp eax,0
    jne .nochange
    jmp .change

.modfour:
    and eax,0x00000fff           ;mod4096
    cmp eax,0
    jne .nochange

.change:
    add edi,r11d
    sub edi,r9d

.nochange:
    pop rcx
    pop rbx
    ret              ;edi指向下一个字
;______________________________________


;进:esi,edi
;出:edi
;用:
;变:
utf8:
    push rcx
    xor rcx,rcx
    mov cl,16
.yihang:
    xor rax,rax
    lodsw
    push rcx
        mov cl,16
      .yidian:
        mov bx,ax
        and bx,0x1
        cmp bx,1
        jne .touming
        mov dword [edi],0x00ff0000
      .touming:
        add edi,r8d
        shr ax,1
        loop .yidian
    sub edi,r10d           ;每个字的行头
    sub edi,r10d           ;每个字的行头
    add edi,r9d            ;下1行
    pop rcx
    loop .yihang

    add edi,r10d            ;下个字的行头
    add edi,r10d            ;下个字的行头
    sub edi,r11d            ;上16行;现在edi=下个字开头

    mov eax,edi
    mov ebx,[0xa028]        ;ebx=vesabase
    sub eax,ebx             ;eax=相对距离

    cmp r8,4
    je .modfour                  ;r8==4
    and eax,0x000003ff          ;r8==3则mod 1024
    cmp eax,0
    jne .nochange
    jmp .change

.modfour:
    and eax,0x00000fff           ;mod4096
    cmp eax,0
.change:
    add edi,r11d
    sub edi,r9d
.nochange:
    pop rcx
    ret              ;edi指向下一个字
