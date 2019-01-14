#http://www.sf.net/projects/gnu-efi
#http://www.sf.net/projects/elilo

all:
	x86_64-w64-mingw32-gcc -c -ffreestanding -Ignu-efi/inc -Ignu-efi/inc/x86_64 -Ignu-efi/inc/protocol cxx/efimain.c -o efimain.o
	x86_64-w64-mingw32-gcc -c -ffreestanding -Ignu-efi/inc -Ignu-efi/inc/x86_64 -Ignu-efi/inc/protocol cxx/main.c -o main.o
	x86_64-w64-mingw32-gcc -c -ffreestanding -Ignu-efi/inc -Ignu-efi/inc/x86_64 -Ignu-efi/inc/protocol gnu-efi/lib/data.c -o data.o
	x86_64-w64-mingw32-gcc -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 -e efi_main efimain.o main.o data.o -lgcc -o ../BOOTX64.EFI
clean:
	rm -f *.o *.efi *.EFI
