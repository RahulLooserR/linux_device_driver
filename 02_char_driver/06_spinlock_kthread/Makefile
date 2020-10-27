obj-m := char_driver_tasklet_kthread_spinlock.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD)  modules

install:
	$(MAKE) -C $(KDIR) M=$(PWD)  modules_install

clean:
	$(MAKE) -C $(KDIR) M=$(PWD)  clean
