cflag := -c -fPIC -fno-stack-protector -O2

.PHONY:rpi
rpi:
	$(CC) $(cflag) rpi/gpu/gpu.c
	$(CC) $(cflag) rpi/kbd/kbd.c
	$(CC) $(cflag) rpi/mbox/mbox.c
	$(CC) $(cflag) rpi/power/power.c
	$(CC) $(cflag) rpi/time/time.c
	$(CC) $(cflag) rpi/stor/ahci.c
	$(CC) $(cflag) rpi/stor/sd.c
	$(CC) $(cflag) rpi/uart/miniuart.c
	$(CC) $(cflag) rpi/plat.c
	$(CC) $(cflag) device.c

.PHONY:x64
x64:
	$(CC) $(cflag) x64/cpu/fp.c
	$(CC) $(cflag) x64/cpu/port.c
	$(CC) $(cflag) x64/pci/pci.c
	$(CC) $(cflag) x64/power/pwr.c
	$(CC) $(cflag) x64/ps2/ps2kbd.c
	$(CC) $(cflag) x64/stor/ahci.c
	$(CC) $(cflag) x64/stor/ide.c
	$(CC) $(cflag) x64/stor/sd.c
	$(CC) $(cflag) x64/uart/8250.c
	$(CC) $(cflag) x64/usb/xhcihost.c
	$(CC) $(cflag) x64/usb/xhciport.c
	$(CC) $(cflag) x64/plat.c
	$(CC) $(cflag) device.c
clean:
	rm -f *.o
