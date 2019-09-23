;[0000,01ff]
incbin "0000-mbr/mbr.bin"
times 0x200-($-$$) db 0


;[0200,3fff]
incbin "0200-realmode/16bit.bin"
times 0x4000-($-$$) db 0


;[4000,7fff]
incbin "4000-longmode/64bit.bin"
times 0x8000-($-$$) db 0
