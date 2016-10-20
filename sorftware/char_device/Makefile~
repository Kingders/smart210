obj-m := chardev.o
KDIR := /usr/src/linux-headers-4.2.0-42-generic
all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	rm -f *.ko *.o *.mod.o *.symvers *.bak *.order
