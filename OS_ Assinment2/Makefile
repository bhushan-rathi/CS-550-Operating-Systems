obj-m += processlist.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 
	gcc -o user_app user_app.c

