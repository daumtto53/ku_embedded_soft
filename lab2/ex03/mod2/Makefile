obj-m := mod2.o

KBUILD_EXTRA_SYMBOLS := /home/cms/cms/embedded_soft/lab2/ex03/mod1/Module.symvers
KERNELDIR = /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default : 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean	:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

