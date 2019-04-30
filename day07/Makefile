obj-m += main.o

all:
	mount -vt debugfs none /sys/kernel/debug || true
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
