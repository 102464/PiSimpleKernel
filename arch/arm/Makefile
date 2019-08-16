# Test

ARCH          = arm
CROSS_COMPILE = arm-elf-

CC      = $(CROSS_COMPILE)gcc-4.7
OBJCOPY = $(CROSS_COMPILE)objcopy

OBJS = init.o exception.o start.o boot.o panic.o \
	   stdoutput.o mailbox.o mailbox_iface.o uart.o \
	   framebuffer.o
	   
LDS  = link.lds

.c.o:
	$(CC) -fpic -ffreestanding \
	-std=gnu99 -c $< -o $@ -g -O2 -Wall -Wextra

.s.o:
	$(CC) $< -o $@ -g

all: $(OBJS)
	@echo "Linking files"
	$(CC) -T $(LDS) -o kernel -mcpu=arm1176jzf-s -ffreestanding -nostdlib -O2 -g $(OBJS) -lgcc
	$(OBJCOPY) kernel -O binary kernel.bin
	@echo "Kernel built"

run:
	qemu-system-arm -M raspi2 -m 256M -serial stdio -kernel kernel

run_debug:
	@echo Running gdb debug server on tcp::1234.
	qemu-system-arm -M raspi2 -m 256M -serial stdio -kernel kernel -S -gdb tcp::1234

clean:
	rm *.o kernel.elf kernel.bin -f

