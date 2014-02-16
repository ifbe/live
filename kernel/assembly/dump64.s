;init/[1,7]
[BITS 64]
;____________before main_________________
start:
    xor rax,rax
    mov al,[0xa019]        ;bit/点
    mov [bitpp],eax

    shr al,3               ;byte/点
    mov [bytepp],eax

    shl eax,10             ;bytes/line
    mov [oneline],eax

    shl eax,4              ;bytes/16line
    mov [sixteenline],eax

    xor rax,rax
    mov [0xff0],rax
    add ax,0x800
    mov [0xff8],rax
    mov r14,rax             ;r14 memory pointer
    xor r15,r15                  ;r15 offset pointer
    jmp forever
;_______________________________________
bytepp:dd 0
oneline:dd 0
bitpp:dd 0
sixteenline:dd 0


;_________________main____________________
forever:
    call ramdump
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
    je esc
    cmp al,0x29
    je command
    cmp al,0x0f
    je tab
    cmp al,0x1c
    je enter
    cmp al,0x4b
    je left
    cmp al,0x4d
    je right
    cmp al,0x48
    je up
    cmp al,0x50
    je down

    mov ebx,[0xff8]
    mov [ebx],al
    inc ebx
    mov [0xff8],bx
    cmp bx,0xff0
    jb forever

    mov word [0xff8],0x800
    jmp forever
;_________________________________


;___________________________________
esc:
    mov dx,[0x4fc]
    mov ax,[0x4fe]
    or ax,0x2000
    out dx,ax
backspace:
    dec word [0xff8]
    mov ebx,[0xff8]
    mov byte [ebx],0
    cmp word [0xff8],0x800
    ja forever
    mov word [0xff8],0x800
    jmp forever
command:
    mov al,[0xff1]
    inc al
    and al,0x01
    mov [0xff1],al
    cmp byte [0xff1],0
    je start
    call 0x100000
    jmp keyboard
tab:
    mov al,[0xff2]
    inc al
    and al,0x01
    mov [0xff2],al
    jmp forever
enter:
    call falsify                ;do something here
    call clearbuffer
    jmp forever
left:
    mov rax,r15
    mov bl,0x30
    div bl
    cmp ah,0
    je .leftleft
    dec r15
    jmp forever
.leftleft:
    sub r14,0x800
    jmp forever
right:
    mov rax,r15
    mov bl,0x30
    div bl
    cmp ah,0x2f
    je .rightright
    inc r15
    jmp forever
.rightright:
    add r14,0x800
    jmp forever
up:
    cmp r15,48
    jb .upup
    sub r15,48
    jmp forever
.upup:
    sub r14,0x30
    jmp forever
down:
    cmp r15,0x83f
    ja .downdown
    add r15,48
    jmp forever
.downdown:
    add r14,0x30
    jmp forever
;________________________________


;_______al为高4位，ah为低4位，把ax翻译成16进制数字____________
;__________1～9找键盘，f1～f6当成abcdef用______________
falsify:
    mov ax,[0x800]
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
    mov ebx,0x800
cb: mov qword [ebx],0
    add bx,8
    cmp bx,0xfe0
    jb cb
    mov word [0xff8],0x800
    ret
;_________________________________


;进:r14
;出:
;用:
;变:
;______________________________
ramdump:
    mov edi,[0xa028]        ;vesabase
    xor eax,eax
    mov ecx,1024*16
yellow:
    mov [edi],eax
    add edi,[bytepp]
    loop yellow

    mov edi,[0xa028]
    mov cl,6
fivetimes:
    add edi,[bitpp]
    loop fivetimes

    mov dl,0
    mov cl,6
sixtimes:
    mov eax,[bitpp]
    shl eax,4
    add edi,eax            ;edi=edi+16*r10
    call displayhex
    add dl,8
    loop sixtimes

    mov edi,[0xa028]
    add edi,[sixteenline]

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
    mov rdx,[0xff0]
    call displayinverted  ;显示键盘状态寄存器:esc,`,tab,cap,shift,ctrl,super,alt
    pop rdi

    add edi,[sixteenline]          ;挪到最后一行
    mov rdx,r15
    call displaypositive
    mov cl,6
kk:
    mov esi,0xb200                ;blank
    call char
    loop kk                      ;6空

    xor rcx,rcx
keyboardbuffer:
    mov dl,[ecx+0x800]
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
    xor ah,ah
    shl ax,4
    mov esi,0xb000                     ;zero
    add si,ax
    ret
;______________________________


;进:edi,r15d
;出:edi
;用:
;变:cl
;_______________________________
pointer:
    mov edi,[0xa028]
    add edi,[sixteenline]

    mov eax,[bytepp]
    shl eax,4
    add edi,eax
    lea edi,[2*eax+edi]                ;跳过地址头
    lea edi,[8*eax+edi]

    mov eax,r15d
    mov bl,48               ;一排48个byte，计算在第几排
    div bl
    mov dl,ah
    xor ah,ah               ;清空之  

    mul byte [bytepp]
    shl eax,14
    add edi,eax              ;哪一行

    mov eax,r15d
    and eax,0x00000007             ;mod8取余
shi:
    cmp al,0
    je jieshu8
    mov ebx,[bitpp]
    shl ebx,1
    add edi,ebx
    dec al
    jmp shi
jieshu8:

    mov edx,[bitpp]
    shl edx,1
    mov eax,r15d
    shr eax,3
    mov bl,6
    div bl
sheng:
    cmp ah,0                     ;mod6取余
    je jieshu6
    lea edi,[8*edx+edi]
    add edi,edx
    dec ah
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
    xor rax,rax
    rol rdx,4
    mov al,dl
    and al,0x0f
    call number
    call char
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
    call displayhex
    ror rdx,8
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
        shl al,1
        jnc .background
        mov dword [edi],0xff00
        jmp .pointdone
      .background:
        mov dword [edi],0
      .pointdone:
        add edi,[bytepp]
        loop .yidian
    sub edi,[bitpp]           ;每个字的行头
    add edi,[oneline]            ;下1行
    pop rcx
    loop .yihang

    add edi,[bitpp]            ;下个字的行头
    sub edi,[sixteenline]            ;上16行;现在edi=下个字开头

    mov eax,edi
    mov ebx,[0xa028]        ;ebx=vesabase
    sub eax,ebx             ;eax=相对距离

    cmp byte [bytepp],4
    je .modfour                  ;bpp==4
    and eax,0x000003ff          ;bpp==3则mod 1024
    cmp eax,0
    jne .nochange
    jmp .change

.modfour:
    and eax,0x00000fff           ;mod4096
    cmp eax,0
    jne .nochange

.change:
    add edi,[sixteenline]
    sub edi,[oneline]

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
        shr ax,1
        jnc .touming
        mov dword [edi],0x00ff0000
      .touming:
        add edi,[bytepp]
        loop .yidian
    sub edi,[bitpp]           ;每个字的行头
    sub edi,[bitpp]           ;每个字的行头
    add edi,[oneline]            ;下1行
    pop rcx
    loop .yihang

    add edi,[bitpp]            ;下个字的行头
    add edi,[bitpp]            ;下个字的行头
    sub edi,[sixteenline]            ;上16行;现在edi=下个字开头

    mov eax,edi
    mov ebx,[0xa028]        ;ebx=vesabase
    sub eax,ebx             ;eax=相对距离

    cmp byte [bytepp],4
    je .modfour                  ;bpp==4
    and eax,0x000003ff          ;bpp==3则mod 1024
    cmp eax,0
    jne .nochange
    jmp .change

.modfour:
    and eax,0x00000fff           ;mod4096
    cmp eax,0
.change:
    add edi,[sixteenline]
    sub edi,[oneline]
.nochange:
    pop rcx
    ret              ;edi指向下一个字
