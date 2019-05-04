#!/bin/sh

if [[ -z $1 ]]; then
    echo "usage: $0: login"
    exit 1
fi

insmod main.ko

len=$(wc -c <<< $1)
len=$(( $len - 1 ))
echo "gcc hell.c -DLOGIN=\"$1\" -DLOGIN_LEN=$len -o hell"
gcc hell.c -DLOGIN=\"$1\" -DLOGIN_LEN=$len -o hell
echo "gcc basic.c -DLOGIN=\"$1\" -DLOGIN_LEN=$len -o basic"
gcc basic.c -DLOGIN=\"$1\" -DLOGIN_LEN=$len -o basic

echo "-----BASIC TEST------"
./basic
echo "-----HELL TEST-------"
./hell

rmmod main.ko
