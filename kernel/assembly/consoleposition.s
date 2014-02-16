.upperdown:
mov ah,0
cmp r15,0x600
jb .leftright
mov ah,1

.leftright:
mov al,r15b
and al,0x20
shr al,5

;mov [0x7c4],ax

mov edi,0x100000     ;all position need base address
;mov edi,[0xa028]     ;all position need base address
;cmp ax,0            ;upperleft
cmp ax,0x0101
je .start
;cmp ax,0x0100       ;lowerleft
cmp ax,0x0001
je .lowerline

mov ebx,[oneline]
shr ebx,1
add edi,ebx           ;upperright and lowerright are right
;cmp ax,0x0001
cmp ax,0x0100
je .start                              ;upperright
;直接往下，不用cmp ax,0x0101           ;lowerright

    .lowerline:
    mov eax,[sixteenline]
    ;mov ebx,24
    ;mul ebx
    shl eax,3
    lea eax,[2*eax+eax]
    add edi,eax

;get edi
