obj-m := simple_spin.o

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o $(obj-m:.o=_reader) $(obj-m:.o=_reader.c)
	gcc -o $(obj-m:.o=_writer) $(obj-m:.o=_writer.c)
	sudo insmod $(obj-m:.o=.ko)
	sudo sh mknod.sh
	sudo ./$(obj-m:.o=_writer)
	sudo ./$(obj-m:.o=_reader)
	sudo rmmod $(obj-m:.o=)
	sudo rm /dev/$(obj-m:.o=_dev)
	sudo dmesg | tail --line=25

clean :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm $(obj-m:.o=_reader) $(obj-m:.o=_writer)

