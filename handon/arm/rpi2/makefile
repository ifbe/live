all:
	arm-none-eabi-gcc -mcpu=cortex-a7 -fpic -ffreestanding -c asm.s -o asm.o
	arm-none-eabi-gcc -mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -c cxx.c -o cxx.o -O2 -Wall -Wextra
	arm-none-eabi-gcc -T link.ld -o kernel.elf -ffreestanding -nostdlib asm.o cxx.o
	arm-none-eabi-objcopy kernel.elf -O binary kernel7.img
qemu:
	qemu-system-arm -m 256 -M raspi2 -serial stdio -kernel kernel.elf
