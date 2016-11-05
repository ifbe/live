;addr=0,size=1000
;[0,fff]
incbin "0.old/cpu.bin"
times 0x1000-($-$$) db 0




;addr=1000,size=1000
;[1000,1fff]
incbin "1000/data.bin"
times 0x2000-($-$$) db 0




;addr=2000,size=2000
;[4000,7fff]
incbin "2000/interrupt.bin"
times 0x4000-($-$$) db 0




;addr=4000,size=c000
;[4000,ffff]
incbin "4000/extremities.bin"
times 0x10000-($-$$) db 0



times 0x100000-($-$$) db 0