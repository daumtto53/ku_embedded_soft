NAME := ch2_mod_201512285
APPNAME := ch2_app_201512285

obj-m := $(NAME).o

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o $(APPNAME) $(APPNAME).c
	sudo insmod $(NAME).ko 
	sudo sh mknod.sh
	sudo ./$(APPNAME)
	sudo rmmod $(NAME)
	sudo rm /dev/ch2_dev
	sudo dmesg | tail --lines=10

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm $(APPNAME)
