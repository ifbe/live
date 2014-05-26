bits 64
section .text

extern initxhci

global start
global where
global incwhere
global cleanwhere




start:

call initxhci
ret




incwhere:
inc byte [rel realwhere]
ret

cleanwhere:
mov qword [rel realwhere],0
ret

where:
mov rax,[rel realwhere]
ret

realwhere:
dq 0
