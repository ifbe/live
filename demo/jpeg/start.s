bits 64
section .text


extern main
global start
global where
global size


start:
call main
ret


where:
lea rax,[rel jpeghere]
ret


size:
mov rax,jpegend-jpeghere
ret


jpeghere:
incbin "jarvis.jpg"
jpegend:
