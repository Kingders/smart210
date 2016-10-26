obj-m := mini210_buttons.o
KDIR := /home/kingders/smart210/kernel/kernel2/linux-3.10.102
all:
	make -C $(KDIR) M=$(PWD) modules  CROSS_COMPILE=arm-linux- ARCH=arm
	arm-linux-gcc app.c -o app
	cp app /home/kingders/smart210/rootfs/rootfs1/
	cp mini210_buttons.ko /home/kingders/smart210/rootfs/rootfs1/
clean:
	rm -f *.ko *.o *.mod.* *.sysmvers modules
.PHONY:clean  
