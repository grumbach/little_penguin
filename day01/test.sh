#!/bin/sh


if [[ -z $1 ]]; then
    echo "usage $0 [module.ko]"
    exit 1
fi

insmod $1

INSMOD=`dmesg | grep -i "hello world"`

rmmod $1

RMMOD=`dmesg | grep -i "cleaning up module"`

if [[ -n $INSMOD ]]; then
    echo "insmod OK"
else
    echo "insmod KO"
fi

if [[ -n $RMMOD ]]; then
    echo "rmmod OK"
else
    echo "rmmod KO"
fi
