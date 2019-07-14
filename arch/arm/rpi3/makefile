all:
	aarch64-elf-as -c asm.s -o asm.o
	aarch64-elf-gcc -ffreestanding -c cxx.c -o cxx.o -O2 -Wall -Wextra
	aarch64-elf-ld -T link.ld -o kernel.elf -nostdlib asm.o cxx.o
	aarch64-elf-objcopy kernel.elf -O binary kernel8.img
qemu:
	qemu-system-aarch64 -M raspi3 -serial null -serial stdio -kernel kernel8.img
clean:
	rm *.o *.elf *.img
