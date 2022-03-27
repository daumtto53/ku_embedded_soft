NAME := ch3_
obj-m := ch3_mod_201512285.o

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o $(NAME)writer_1 $(NAME)writer_1.c 
	gcc -o $(NAME)writer_2 $(NAME)writer_2.c 
	gcc -o $(NAME)reader_1 $(NAME)reader_1.c 
	gcc -o $(NAME)reader_2 $(NAME)reader_2.c 
	sudo insmod $(obj-m:.o=.ko)
	sudo sh mknod.sh
	sudo ./ch3_writer_1
	sudo ./ch3_writer_2
	sudo ./ch3_reader_1
	sudo ./ch3_reader_2
	sudo rmmod $(obj-m:.o=)
	sudo rm /dev/$(NAME)dev
	dmesg | tail --lines=50

clean :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm $(NAME)writer_1
	rm $(NAME)reader_1
	rm $(NAME)writer_2
	rm $(NAME)reader_2

