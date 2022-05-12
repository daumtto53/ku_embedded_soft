obj-m:=simple_sensor.o
KERNELDIR:=~/linux-rpi/
ARM:= ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD:=$(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) modules
	arm-linux-gnueabi-gcc -o $(obj-m:.o=_app) $(obj-m:.o=_app.c)
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) clean
	rm $(obj-m:.o=_app)
scp:
	scp $(obj-m:.o=.ko) mknod.sh $(obj-m:.o=_app) pi@10.10.10.12:~/
