;addr=0,size=4000
;[0,3fff]
incbin "0.mbr/head.bin"




;addr=4000,size=4000
;[4000,7fff]
times 0x4000-($-$$) db 0
incbin "1.asm/asm.bin"




;addr=8000,size=8000
;[8000,ffff]
times 0x8000-($-$$) db 0
incbin "2.cxx/cxx.bin"





;64KB
times 0x10000-($-$$) db 0