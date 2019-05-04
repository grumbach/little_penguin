make
insmod main.ko || rmmod main; insmod main.ko
ls -l --color /sys/kernel/debug/fortytwo/

#-rw-r--r-- 1 root root 0 Apr 30 17:53 foo
#-rw-rw-rw- 1 root root 0 Apr 30 17:53 id
#-r--r--r-- 1 root root 0 Apr 30 17:53 jiffies
