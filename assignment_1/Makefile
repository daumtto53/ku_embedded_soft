obj-m := ku_ipc.o

KERNELDIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)

default :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o $(obj-m:.o=_lib_snd) $(obj-m:.o=_lib_snd.c)
	gcc -o $(obj-m:.o=_lib_rcv) $(obj-m:.o=_lib_rcv.c)
clean :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm $(obj-m:.o=lib_snd) $(obj-m:.o=lib_rcv)

