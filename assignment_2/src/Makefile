obj-m:=ku_dispenser.o
KERNELDIR:=~/linux-rpi/
ARM:= ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD:=$(shell pwd)/driver

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) modules
	arm-linux-gnueabi-gcc $(obj-m:.o=_app.c) -o $(obj-m:.o=_app)
	gcc -o ku_app ku_app.c ku_lib.c

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) clean
	rm $(obj-m:.o=_app)
	rm ku_app

scp:
	scp $(obj-m:.o=.ko) mknod.sh $(obj-m:.o=_app) pi@10.10.10.12:~/
