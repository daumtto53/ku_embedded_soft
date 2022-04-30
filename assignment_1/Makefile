obj-m := ku_ipc.o

KERNELDIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)

default :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o $(obj-m:.o=_lib_1) $(obj-m:.o=_lib.c)
	gcc -o $(obj-m:.o=_lib_2) $(obj-m:.o=_lib.c)
clean :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm $(obj-m:.o=_lib_1) $(obj-m:.o=_lib_2)

