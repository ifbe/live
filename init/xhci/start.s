bits 64
section .text

extern initxhci

global start




start:

call initxhci
ret
