obj-m := keyboard_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
install: all
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install
	cp -f *.rules /etc/udev/rules.d/
uninstall:
	rm /etc/udev/rules.d/*.rules
