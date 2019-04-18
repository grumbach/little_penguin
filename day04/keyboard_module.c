#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */
#include <linux/usb.h>      /* Needed for usb */
#include <uapi/linux/hid.h> /* Needed for USB_INTERFACE macros */

static struct usb_device_id kbd_table [] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,    \
			USB_INTERFACE_SUBCLASS_BOOT,     \
			USB_INTERFACE_PROTOCOL_KEYBOARD) },
	{ }
};

MODULE_DEVICE_TABLE(usb, kbd_table);

static int kbd_probe(struct usb_interface *interface,
		      const struct usb_device_id *id)
{
	printk(KERN_INFO "Hello keyboard !\n");
	return 0;
}

static void kbd_disconnect(struct usb_interface *interface)
{
	printk(KERN_INFO "Goodbye keyboard !\n");
}

static struct usb_driver kbd_driver = {
	.name        = "kbd-detect",
	.probe       = kbd_probe,
	.disconnect  = kbd_disconnect,
	.id_table    = kbd_table,
};

static int __init keyboard_init(void)
{
	printk(KERN_INFO "Begin looking for keyboards\n");
	return usb_register(&kbd_driver);
}

static void __exit keyboard_exit(void)
{
	printk(KERN_INFO "Stop looking for keyboards\n");
	usb_deregister(&kbd_driver);
}

module_init(keyboard_init);
module_exit(keyboard_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("agrumbac");
MODULE_DESCRIPTION("This module detects when a keyboard is plugged in!");
