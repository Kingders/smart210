obj-m := bus.o device.o driver.o
KERNEL_DIR := /home/kingders/smart210/kernel/kernel2/linux-3.10.102  
all:
	make -C $(KERNEL_DIR) M=$(PWD) modules  CROSS_COMPILE=arm-linux- ARCH=arm
	arm-linux-gcc app.c -o app
	cp app /home/kingders/smart210/rootfs/rootfs1/
	cp bus.ko /home/kingders/smart210/rootfs/rootfs1/
	cp driver.ko /home/kingders/smart210/rootfs/rootfs1/
	cp device.ko /home/kingders/smart210/rootfs/rootfs1/
clean:
	rm -f *.ko *.o *.mod.o *.symvers *.bak *.order
.PHONY:clean  
