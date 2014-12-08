void idtinstall(irqnum,serveraddr)
{
	stosw
mov dword [edi],0x8e000008
add edi,4
shr rax,16
stosw
shr rax,16
stosd
xor eax,eax
stosd
}


init()
{
lea rax,[rel serveroftimer]
mov edi,0x1200
call idtinstaller

mov edi,0xfee00000
mov dword [edi+0x320],0x20020       ;timer vector
mov dword [edi+0x3e0],0x8          ;devide value
mov dword [edi+0x380],0xffffff          ;init value
sti

}