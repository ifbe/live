;[0000,0fff]
incbin "16bit/16bit.bin"
times 0x1000-($-$$) db 0

;[1000,1fff]
incbin "64bit/64bit.bin"
times 0x2000-($-$$) db 0
