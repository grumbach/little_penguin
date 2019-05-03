#!/bin/sh

eval `grep CONFIG_LOCALVERSION_AUTO=y config*`

if [[ $CONFIG_LOCALVERSION_AUTO -eq "y" ]]; then
    echo "CONFIG_LOCALVERSION_AUTO OK"
else
    echo "CONFIG_LOCALVERSION_AUTO KO"
fi
