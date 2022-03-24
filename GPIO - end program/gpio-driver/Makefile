obj-m += lll-gpio-driver.o

#produce with kernel build system a file called low level learning gpio driver

KDIR = /lib/modules/$(shell uname -r)/build

#kernel directory is equal to build system for linux system


all:

	make -C $(KDIR) M=$(shell pwd) modules

#sub make system - module folder is the current directory and this is where we are going to build the kernel modules

clean:

	make -C $(KDIR) M=$(shell pwd) clean

#say we need to leave and rebuild 
