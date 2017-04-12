obj-m = msgq_test.o
mod_file= Module.symvers
req_file= ../drv/Module.symvers

all: $(mod_file) 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

$(mod_file): $(req_file)
	cp $(req_file) $(mod_file)

$(req_file):
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/../drv/ modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
