obj-m := ch1_mod1_201512285.o

KERNELDIR = /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean : 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean 
