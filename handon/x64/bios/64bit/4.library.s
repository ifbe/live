[bits 64]
%define thatport 0xffc
%define thatdata 0xffe




turnoff:
	mov dx,[thatport]
	mov ax,[thatdata]
	or ax,0x2000
	out dx,ax




reboot:
	mov al,0xfe
	out 0x64,al
