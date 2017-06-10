obj-m += linepipe.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 
	gcc -o consumer consumer.c
	gcc -o producer producer.c
