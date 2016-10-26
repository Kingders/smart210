obj-m := wait_queue.o
#  KDIR := /usr/src/linux-headers-4.2.0-42-generic
KERNEL_DIR := /lib/modules/$(shell uname -r)/build  
PWD := $(shell pwd) 
all:
#	make -C $(KDIR) M=$(PWD) modules
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules  

clean:
	rm -f *.ko *.o *.mod.o *.symvers *.bak *.order
.PHONY:clean  
