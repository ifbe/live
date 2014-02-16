[bits 64]
;___________________8259_____________________
pic:
cli

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

mov al,0xff
out 0x21,al
out 0xa1,al
;____________________________________


;__________________idt____________________
mov word [idtptr.length],0xfff
mov qword [idtptr.base],0x1000
lidt [idtptr]
jmp exceptioninstall

ALIGN 16
idtptr:
.length dw 0
.base dq 0
;_____________________________________


;_______________null exception______________________
exceptioninstall:
mov edi,0x1000

mov rax,exception0
call idtinstall
mov rax,exception1
call idtinstall
mov rax,exception2
call idtinstall
mov rax,exception3
call idtinstall
mov rax,exception4
call idtinstall
mov rax,exception5
call idtinstall
mov rax,exception6
call idtinstall
mov rax,exception7
call idtinstall
mov rax,exception8
call idtinstall
mov rax,exception9
call idtinstall
mov rax,exceptiona
call idtinstall
mov rax,exceptionb
call idtinstall
mov rax,exceptionc
call idtinstall
mov rax,exceptiond
call idtinstall
mov rax,exceptione
call idtinstall
mov rax,exceptionf
call idtinstall
mov rax,exception10
call idtinstall
mov rax,exception11
call idtinstall
mov rax,exception12
call idtinstall
mov rax,exception13
call idtinstall
mov rax,exception14
call idtinstall
mov rax,exception15
call idtinstall
mov rax,exception16
call idtinstall
mov rax,exception17
call idtinstall
mov rax,exception18
call idtinstall
mov rax,exception19
call idtinstall
mov rax,exception1a
call idtinstall
mov rax,exception1b
call idtinstall
mov rax,exception1c
call idtinstall
mov rax,exception1d
call idtinstall
mov rax,exception1e
call idtinstall
mov rax,exception1f
call idtinstall

mov ecx,0xe0
external:
mov rax,lastwords
call idtinstall
loop external

jmp exceptiondone
;_______________________________________


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
exception0:
mov byte [0xffff],0
jmp lastwords

exception1:
mov byte [0xffff],1
jmp lastwords

exception2:
mov byte [0xffff],2
jmp lastwords

exception3:
mov byte [0xffff],3
jmp lastwords

exception4:
mov byte [0xffff],4
jmp lastwords

exception5:
mov byte [0xffff],5
jmp lastwords

exception6:
mov byte [0xffff],6
jmp lastwords

exception7:
mov byte [0xffff],7
jmp lastwords

exception8:
mov byte [0xffff],8
jmp lastwords

exception9:
mov byte [0xffff],9
jmp lastwords

exceptiona:
mov byte [0xffff],0xa
jmp lastwords

exceptionb:
mov byte [0xffff],0xb
jmp lastwords

exceptionc:
mov byte [0xffff],0xc
jmp lastwords

exceptiond:
mov byte [0xffff],0xd
jmp lastwords

exceptione:
mov byte [0xffff],0xe
jmp lastwords

exceptionf:
mov byte [0xffff],0xf
jmp lastwords

exception10:
mov byte [0xffff],0x10
jmp lastwords

exception11:
mov byte [0xffff],0x11
jmp lastwords

exception12:
mov byte [0xffff],0x12
jmp lastwords

exception13:
mov byte [0xffff],0x13
jmp lastwords

exception14:
mov byte [0xffff],0x14
jmp lastwords

exception15:
mov byte [0xffff],0x15
jmp lastwords

exception16:
mov byte [0xffff],0x16
jmp lastwords

exception17:
mov byte [0xffff],0x17
jmp lastwords

exception18:
mov byte [0xffff],0x18
jmp lastwords

exception19:
mov byte [0xffff],0x19
jmp lastwords

exception1a:
mov byte [0xffff],0x1a
jmp lastwords

exception1b:
mov byte [0xffff],0x1b
jmp lastwords

exception1c:
mov byte [0xffff],0x1c
jmp lastwords

exception1d:
mov byte [0xffff],0x1d
jmp lastwords

exception1e:
mov byte [0xffff],0x1e
jmp lastwords

exception1f:
mov byte [0xffff],0x1f
jmp lastwords
;________________________________________


;________________________________________________
lastwords:
mov [0xffb0],rsp
mov rsp,0xfff8

push rax                 ;[0xfff0]
push qword 1
push rcx                 ;[0xffe0]
push qword 2
push rdx                 ;[0xffd0]
push qword 3
push rbx                 ;[0xffc0]
push qword 4

sub rsp,0x8

push qword 5
push rbp                ;[0xffa0]
push qword 6
push rsi                ;[0xff90]
push qword 7
push rdi                ;[0xff80]

push qword 8
push r8                 ;[0xff70]
push qword 9
push r9                 ;[0xff60]
push qword 0xa
push r10                ;[0xff50]
push qword 0xb
push r11                ;[0xff40]
push qword 0xc
push r12                ;[0xff38]
push qword 0xd
push r13                ;[0xff20]
push qword 0xe
push r14                ;[0xff10]
push qword 0xf
push r15                ;[0xff00]

push qword 0
mov rax,cr0
push rax                ;[0xfef0]
push qword 0xff
pushf                   ;[0xfee0]
push qword 2
mov rax,cr2
push rax                ;[0xfed0]
push qword 3
mov rax,cr3
push rax                ;[0xfec0]
push qword 4
mov rax,cr4
push rax                ;[0xfeb0]
push qword 8
mov rax,cr8
push rax                ;[0xfea0]

sub rsp,0x20              ;no cr?,cr?

push qword 0
mov rax,dr0             ;[0xfe70]
push rax
push qword 1
mov rax,dr1             ;[0xfe60]
push rax
push qword 2
mov rax,dr2             ;[0xfe50]
push rax
push qword 3
mov rax,dr3             ;[0xfe40]
push rax

sub rsp,0x20            ;no dr4,dr5

push qword 6
mov rax,dr6             ;[0xfe10]
push rax
push qword 7
mov rax,dr7             ;[0xfe00]
push rax

xor rax,rax
mov [0xb000],rax
mov [0xb008],rax

mov rsi,[0xffb0]
mov rcx,0
.oldstack:
push rsi
lodsq
push rax                ;[0xfe00,0xfeff]
inc rcx
cmp rcx,0x10
jb .oldstack

mov rbx,0xffff
.continue:
mov edi,[0xa028]
.first:                  ;行
mov ax,bx
movzx eax,ax
not ax
shr ax,4
mul byte [0xa019]        ;*bpp*8
shl eax,11               ;*(1024/8)*16*2
add edi,eax

.second:                 ;列
mov ax,bx
movzx eax,ax
not ax
and ax,0x0f
mul byte [0xa019]         ;*bpp*8*2
shl eax,1
add edi,eax

.startdump:
xor rax,rax
mov al,[rbx]
and al,0xf0
mov rsi,rax
add rsi,0xb000
call hex

mov al,[0xa019]
movzx eax,al
add edi,eax
shl eax,11
sub edi,eax

xor rax,rax
mov al,[rbx]
shl al,4
mov rsi,rax
add rsi,0xb000
call hex

dec rbx
cmp rbx,0xfd00
jae .continue

hlt


hex:        ;a,c,d,si,di
mov ecx,16
.first:
xor dx,dx
mov dl,[esi]
inc esi
    ;___________ 
    .second:
    xor rax,rax
    shl dl,1
    jnc .next
    not rax
    .next:
    mov [edi],eax
    mov al,[0xa019]
    movzx eax,al
    shr al,3
    add edi,eax
    inc dh
    cmp dh,8
    jb .second
    ;___________

mov al,[0xa019]
movzx eax,al
sub edi,eax
shl eax,7
add edi,eax
loop .first

ret
;__________________________________________________


exceptiondone:
