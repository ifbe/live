bits 64




section .text
global fileaddr
global filesize
fileaddr:
	lea rax,[rel jpegstart]
	ret
filesize:
	mov rax,jpegend-jpegstart
	ret




section .data
jpegstart:
incbin "../jarvis.jpg"
jpegend:
