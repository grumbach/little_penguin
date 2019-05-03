#!/bin/sh

if [[ -z $1 ]]; then
    echo "usage: $0 [file.c]"
    exit 1
fi

if [[ -z "$(grep USB_INTERFACE_INFO $1)" ||
	  -z "$(grep USB_INTERFACE_PROTOCOL_KEYBOARD $1)" ||
	      -z "$(grep USB_INTERFACE_CLASS_HID $1)" ]]; then
    echo "generic keyboard KO"
else
    echo "generic keyboard OK"
fi

if [[ -z "$(grep .probe $1)" ||
	  -z "$(grep .disconnect $1)" ]]; then
    echo "probe disconnect KO"
else
    echo "probe disconnect OK"
fi

if [[ -z "$(grep usb_register $1)" ]]; then
    echo "register KO"
else
    echo "register OK"
fi

if [[ -z "$(grep usb_deregister $1)" ]]; then
    echo "deregister KO"
else
    echo "deregister OK"
fi

if [[ -z "$(grep MODULE_DEVICE_TABLE $1)" ]]; then
    echo "MODULE_DEVICE_TABLE KO"
else
    echo "MODULE_DEVICE_TABLE OK"
fi
