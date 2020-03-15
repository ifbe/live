cflag := -c -fPIC -fno-stack-protector -O2

binary:
	gcc $(cflag) start.c
	make pcobj CC=gcc
	make pcbin LD=ld
cross:
	x86_64-elf-gcc $(cflag) start.c
	make pcobj CC=x86_64-elf-gcc
	make pcbin LD=x86_64-elf-ld
pcobj:
	make -s -C libboot0 x64 CC=$(CC)
	make -s -C libboot1 CC=$(CC)
	make -s -C libhard0 x64 CC=$(CC)
	make -s -C libhard1 CC=$(CC)
	make -s -C libsoft0 CC=$(CC)
	make -s -C libsoft1 CC=$(CC)
	make -s -C libuiux0 CC=$(CC)
	make -s -C libuiux1 CC=$(CC)
pcbin:
	$(LD) -T pc64.ld \
	start.o \
	libboot0/*.o libboot1/*.o libhard0/*.o libhard1/*.o \
	libsoft0/*.o libsoft1/*.o libuiux0/*.o libuiux1/*.o \
	-o cxx.bin

efi:
	make -s -C libboot0 efi CC=$(CC)
	make -s -C libboot1 CC=$(CC)
	make -s -C libhard0 efi CC=$(CC)
	make -s -C libhard1 CC=$(CC)
	make -s -C libsoft0 CC=$(CC)
	make -s -C libsoft1 CC=$(CC)
	make -s -C libuiux0 CC=$(CC)
	make -s -C libuiux1 CC=$(CC)
	x86_64-w64-mingw32-gcc -nostdlib -shared \
	-Wl,-dll -Wl,--subsystem,10 -e efi_main -o BOOTX64.EFI \
	libboot0/*.o libboot1/*.o libhard0/*.o libhard1/*.o \
	libsoft0/*.o libsoft1/*.o libuiux0/*.o libuiux1/*.o \
	-lgcc

pi64test:
	qemu-system-aarch64 -M raspi3 -serial null -serial stdio -kernel kernel8.img
pi32:
	make piobj CC=arm-none-eabi-gcc
	arm-none-eabi-gcc -T pi32.ld -ffreestanding -nostdlib \
	libboot0/*.o libboot1/*.o libhard0/*.o libhard1/*.o \
	libsoft0/*.o libsoft1/*.o libuiux0/*.o libuiux1/*.o \
	-o kernel.elf
	arm-none-eabi-objcopy kernel.elf -O binary kernel7.img
pi64:
	make piobj AS=aarch64-elf-as CC=aarch64-elf-gcc
	aarch64-elf-ld -T pi64.ld -nostdlib \
	libboot0/*.o libboot1/*.o libhard0/*.o libhard1/*.o \
	libsoft0/*.o libsoft1/*.o libuiux0/*.o libuiux1/*.o \
	-o kernel.elf
	aarch64-elf-objcopy kernel.elf -O binary kernel8.img
piobj:
	make -s -C libboot0 rpi AS=$(AS) CC=$(CC) \
cflag="-ffreestanding -fno-stack-protector -Wall -Wextra -c -O2"
	make -s -C libboot1 AS=$(AS) CC=$(CC) \
cflag="-ffreestanding -fno-stack-protector -Wall -Wextra -c -O2"
	make -s -C libhard0 rpi AS=$(AS) CC=$(CC) \
cflag="-ffreestanding -fno-stack-protector -Wall -Wextra -c -O2"
	make -s -C libhard1 AS=$(AS) CC=$(CC) \
cflag="-ffreestanding -fno-stack-protector -Wall -Wextra -c -O2"
	make -s -C libsoft0 AS=$(AS) CC=$(CC) \
cflag="-ffreestanding -fno-stack-protector -Wall -Wextra -c -O2"
	make -s -C libsoft1 AS=$(AS) CC=$(CC) \
cflag="-ffreestanding -fno-stack-protector -Wall -Wextra -c -O2"
	make -s -C libuiux0 AS=$(AS) CC=$(CC) \
cflag="-ffreestanding -fno-stack-protector -Wall -Wextra -c -O2"
	make -s -C libuiux1 AS=$(AS) CC=$(CC) \
cflag="-ffreestanding -fno-stack-protector -Wall -Wextra -c -O2"

clean:
	rm -f *.o *.bin *.elf *.img
	make -C libboot0 clean
	make -C libboot1 clean
	make -C libhard0 clean
	make -C libhard1 clean
	make -C libsoft0 clean
	make -C libsoft1 clean
	make -C libuiux0 clean
	make -C libuiux1 clean
