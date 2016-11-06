;addr=0,size=1000
;[0,fff]
incbin "0.bios/cpu.bin"
times 0x1000-($-$$) db 0




;addr=1000,size=1000
;[1000,1fff]
incbin "1.asm/data.bin"
times 0x2000-($-$$) db 0




;addr=2000,size=2000
;[4000,7fff]
incbin "1.asm/interrupt.bin"
times 0x4000-($-$$) db 0




;addr=4000,size=c000
;[4000,ffff]
incbin "2.ccc/extremities.bin"
times 0x10000-($-$$) db 0
