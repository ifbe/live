ORG 0x7C00                     ;这是一个失败的....载入超过4k就出问题....
BITS 16                        ;直接嵌入扇区的那个....正常载入16k....
    cli
    xor eax,eax    ; Set up segment registers.
    mov ds, ax
    mov es, ax
    mov ss, ax    ; Set up stack so that it starts below Main.
    mov sp, 0x7c00

    ;check for extension support
;________________________________________________
    mov ah,0x41
    mov bx,0x55aa
    int 0x13
    mov si,bios
    jc print            ;no extension
;_______________________________________________________


;____________0xc00=mbr_____________________________
    mov eax,0
    mov [dap.blocknumber],eax
    mov byte [dap.blockcount],1
    mov word [dap.bufferoffset],0xc00
    call loadsector
;__________________________________________________


    ;check mbr for fat32
;_______________________________________________
    mov cx,0x04
    mov di,0x0db2
search:
    add di,0x10
    mov al,[di]
    cmp al,0x0b
    je dbr          ;find fat32
    loop search
    mov si,part
    jmp print       ;fat32 is not found
;__________________________________________________


;_______________0xe00=dbr_______________________
dbr:
    mov eax,[di+4]
    mov [dap.blocknumber],eax
    mov byte [dap.blockcount],1
    mov word [dap.bufferoffset],0xe00
    call loadsector
;_______________________________________________


    ;calculate for FAT1 sector
;________________________________
    mov eax,[0xe1c]           ;base sector
    add ax,[0xe0e]            ;+reserved sectors
    ;-----------now eax=[0x7e00]=sector for first FAT table---------
    mov [0x7e00],eax
;_________________________________________________


    ;calculate for root sector
;_________________________________________________
    mov cl,[0xe10]            ;how many FAT table
countfat:
    add eax,[0xe24]           ;+ fat1
    loop countfat             ;+ fat2..........
    mov [0x7e10],eax          ;save cluster2 to 0x7e10
    ;-----------now eax=first cluster=cluster2--------------
    mov bl,[0xe0d]            ;sectors per cluster
    mov [0x7e20],bl           ;save it to 0x7e20
;________________________________________________________


    ;load root to 0x2000
;_____________________________________________
    mov [dap.blocknumber],eax
    mov bl,[0x7e20]
    mov byte [dap.blockcount],bl
    mov word [dap.bufferoffset],0x2000
    call loadsector
;__________________________________________________


;_________function read____________
    mov edx,"LIVE"
    call read
;__________________________________


    jmp 0x0000:0xc000


;______________load sector_________________________
loadsector:
    pushad
    mov ah,0x42
    mov dl,0x80
    mov si,dap
    int 0x13
    mov si,load
    jc print
    popad
    ret
;_________________________________________________


    ;load FAT to 0x1000~0x2000
;______________________________________
loadfat:                       ;esi=cluster number
    push esi
    mov cl,10        ;esi/0x80=对应扇区  扇区/8  0x8*0x80=0x400=1024=2^10
    shr esi,cl       ;how many 8 sectors
    lea esi,[esi*8]  ;esi*8
    add esi,[0x7e00]           ;FAT sector
    mov [0x7e50],esi
    mov [dap.blocknumber],esi
    mov byte [dap.blockcount],0x8        ;8个扇区总共4Kbyte,1024个项.
    mov word [dap.bufferoffset],0x1000
    call loadsector

    pop esi
    lea esi,[esi*4]
    mov cl,20
    shl esi,cl
    shr esi,cl
    add esi,0x1000
    mov [0x7e60],esi
    ret
;_______________________________________________


;______________in:  edx=name______________
read:
    ;search file called live
    ;_______________________________________________
    mov ebp,0x2000
    mov cx,0x80
.searchfile:
    cmp dword [ebp],edx
    je .findfile               ;find it
    add ebp,0x20
    loop .searchfile           ;16 times
    mov si,file               ;not find at last
    jmp print


.findfile:
    mov [0x7e30],ebp          ;save file address
    mov ax,[ebp+0x1a]
    mov [0x7e40],ax           ;cluster low
    mov ax,[ebp+0x14]
    mov [0x7e42],ax           ;cluster high
    ;-----------now 0x7e40 is cluster for "live"-------------------
;_________________________________________________


.loadfile:
;______________________________________________
    xor ebx,ebx
    mov bl,[0x7e20]           ;sectors per cluster
    mov di,0xc000            ;destination
    mov eax,[0x7e40]          ;origin cluster for file

.continue:                         ;eax=cluster&&di=buffer
    push eax
    sub eax,2                      ;sub......2......
    mul ebx                        ;offset
    add eax,[0x7e10]               ;base
    mov [dap.blocknumber],eax
    mov byte [dap.blockcount],bl
    mov word [dap.bufferoffset],di
    call loadsector

    pop esi
    call loadfat
    add di,0x200          ;bufferaddress+512byte
    mov eax,[esi]         ;eax=next cluster

    cmp eax,0x0fffffff
    jne .continue

.done:
    ret
;______________________________________________


print:
;__________________________________________________
    lodsb         ;Load the value at [@es:@si] in @al.
    test al, al   ;If AL is the terminator character,stop printing.
    je die
    mov ah, 0x0E	
    int 0x10
    jmp print     ; Loop till the null character not found.
;_____________________________________________


die:hlt
    jmp die

dap:
.packagesize db 0x10
.reserved db 0x00
.blockcount dw 0x0002
.bufferoffset dw 0x8000
.buffersegment dw 0x0000
.blocknumber dq 0x0000000000000001

load: db "load",0
part: db "part",0
bios: db "bios",0
file: db "file",0

times 510-($-$$)db 0
dw 0xaa55
