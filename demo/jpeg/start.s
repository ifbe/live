bits 64

global start
global where
global size
extern main




section .text

start:
call main
ret

where:
lea rax,[rel jpeghere]
ret

size:
mov rax,jpegend-jpeghere
ret




section .data

jpeghere:
incbin "jarvis.jpg"
jpegend:
