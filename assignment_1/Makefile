obj-m := ku_ipc.o

KERNELDIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)

default :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o $(obj-m:.o=_lib_snd) $(obj-m:.o=_lib_snd.c)
	gcc -o $(obj-m:.o=_lib_rcv) $(obj-m:.o=_lib_rcv.c)
	gcc -o $(obj-m:.o=_lib_snd1) $(obj-m:.o=_lib_snd1.c)
	gcc -o $(obj-m:.o=_lib_snd2) $(obj-m:.o=_lib_snd2.c)
	gcc -o $(obj-m:.o=_lib_snd3) $(obj-m:.o=_lib_snd3.c)
	gcc -o $(obj-m:.o=_lib_rcv1) $(obj-m:.o=_lib_rcv1.c)
	gcc -o $(obj-m:.o=_lib_rcv2) $(obj-m:.o=_lib_rcv2.c)

clean :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm $(obj-m:.o=_lib_snd) $(obj-m:.o=_lib_rcv) 
	rm $(obj-m:.o=_lib_snd1) 	$(obj-m:.o=_lib_snd2) $(obj-m:.o=_lib_snd3) $(obj-m:.o=_lib_rcv1) $(obj-m:.o=_lib_rcv2)
