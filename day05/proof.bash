make
insmod main.ko
gcc test.c -DLOGIN=\"agrumbac\" -DLOGIN_LEN=$(wc -c <<< "agrumbac")
./a.out
rmmod main

#TESTING open device forty twoOK
#TESTING read device forty twoOK
#TESTING write agrumbac device forty twoagrumbac
#OK
#TESTING bad write device forty twoOK
