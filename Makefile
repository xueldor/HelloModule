KVERS = $(shell uname -r)

#Kernel modules
obj-m += hello.o

build:kernel_modules

kernel_modules:
	$(info $(KVERS))
	$(info $(CURDIR))
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules

clean: 
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean
