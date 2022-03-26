obj-m := simple_driver.o

KERNELDIR = /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o simple_driver_test simple_driver_test.c
	sudo insmod simple_driver.ko
	sudo sh mknod.sh
	sudo ./simple_driver_test
	sudo rmmod simple_driver
	sudo rm /dev/simple_driver_dev
	dmesg | tail --lines=10


clean :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm simple_driver_test
