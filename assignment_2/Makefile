obj-m:=ku_dispenser.o
KERNELDIR:=~/linux-rpi/
ARM:= ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD:=$(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) modules
	arm-linux-gnueabi-gcc -o ku_app ku_app.c ku_lib.c

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) clean
	rm ku_app

scp:
	scp $(obj-m:.o=.ko) mknod.sh ku_app pi@10.10.10.12:~/
