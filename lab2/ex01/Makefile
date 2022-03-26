obj-m := simple_driver.o

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o $(obj-m:.o=_app) $(obj-m:.o=_app.c)
	sudo insmod $(obj-m:.o=.ko)
	sudo sh mknod.sh
	sudo ./$(obj-m:.o=_app)
	sudo rmmod $(obj-m:.o=)
	sudo rm /dev/$(obj-m:.o=_dev)
	sudo dmesg | tail --lines=10

clean :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm $(obj-m:.o=_app)
