NAME := simple_param

obj-m := $(NAME).o

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default : 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o $(NAME)_app $(NAME)_app.c
	sudo insmod $(NAME).ko my_id="123"
	sudo sh mknod.sh
	sudo ./$(NAME)_app
	sudo rmmod $(NAME)
	sudo rm /dev/$(NAME)_dev
	sudo dmesg | tail --lines=10


clean :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm $(NAME)_app
