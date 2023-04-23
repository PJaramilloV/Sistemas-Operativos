ccflags-y := -Wall

obj-m := buffer.o
buffer-objs := kmutex.o buffer-impl.o

KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

#include $(KDIR)/.config

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

buffer-impl.o kmutex.o: kmutex.h

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	sudo rm /dev/buffer 

ins:
	sudo mknod /dev/buffer c 61 0
	sudo chmod a+rw /dev/buffer

on:
	sudo insmod $(PWD)/buffer.ko
	dmesg | grep buffer | tail -n 3

off:
	sudo rmmod $(PWD)/buffer.ko