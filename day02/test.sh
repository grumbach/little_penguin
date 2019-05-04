#!/bin/sh

BOOT=`grep -- -thor_kernel *.log`
PATCH=`grep -E '\-EXTRAVERSION|\+EXTRAVERSION' Makefile.patch`

if [[ -n $BOOT ]]; then
    echo "kernel log OK"
else
    echo "kernel log KO"
fi

if [[ -n $PATCH ]]; then
    echo "patch OK"
else
    echo "patch KO"
fi
