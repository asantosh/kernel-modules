obj-m = msgq.o
mod_dir:=$(PWD)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(mod_dir) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(mod_dir) clean
