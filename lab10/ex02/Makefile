obj-m:=simple_speaker.o
KERNELDIR:=~/linux-rpi/
ARM:= ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD:=$(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) clean
scp:
	scp simple_speaker.sh $(obj-m:.o=.ko) pi@10.10.10.12:~/

