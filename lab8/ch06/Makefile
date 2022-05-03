obj-m := ch6_mod.o

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	arm-linux-gnueabi-gcc $(obj-m:.o=_app.c) -o $(obj-m:.o=_app)

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm $(obj-m:.o=_app)

scp:
	scp $(obj-m:.o=.ko) mknod.sh $(obj-m:.o=_app) pi@10.10.10.12:~/

