all:
	arm-none-eabi-gcc -mcpu=arm1176jzf-s -fpic -ffreestanding -c asm.s -o asm.o
	arm-none-eabi-gcc -mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -c cxx.c -o cxx.o -O2 -Wall -Wextra
	arm-none-eabi-gcc -T link.ld -o kernel.elf -ffreestanding -O2 -nostdlib asm.o cxx.o
	arm-none-eabi-objcopy kernel.elf -O binary kernel7.img
