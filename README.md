# little_penguin
Linux Kernel module programming assignments

## About Linux Kernel Module Programming

### Inserting and Removing (`man lsmod; man insmod; man rmmod`)

| Program            | Black Sheep Wall                               |
|--------------------|------------------------------------------------|
| `lsmod`            | show the status of modules in the Linux Kernel |
| `insmod module.ko` | insert a module into the Linux Kernel          |
| `rmmod module.ko`  | remove a module from the Linux Kernel          |

### Compiling with a Makefile (`linux/Documentation/kbuild/modules.txt`)

```Makefile
# single file (<module_name>.c)
obj-m := <module_name>.o

# multiple files
<module_name>-y := <src1>.o <src2>.o ...
ccflags-y := -I<include dir>

## rules
all:
	make −C /lib/modules/$(shell uname −r)/build M=$(PWD) modules
clean:
	make −C /lib/modules/$(shell uname −r)/build M=$(PWD) clean
```

### Init and Exit

```c
#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h>   /* Needed for the macros */

static int data __initdata = 42;

static int __init hello_init(void)
{
	printk(KERN_INFO "Hello, world %d\n", data);
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodbye, world\n");
}

module_init(hello_init);
module_exit(hello_exit);
```

### Licensing and Documentation

```c
MODULE_LICENSE("GPL");
MODULE_AUTHOR("agrumbac");
MODULE_DESCRIPTION("This module does mind blowing things!");
MODULE_SUPPORTED_DEVICE("testdevice");
```

### Command line arguments

```c
/*
 * module_param(foo, int, 0000)
 * The first param is the parameters name
 * The second param is it's data type
 * The final argument is the permissions bits,
 * for exposing parameters in sysfs (if non−zero) at a later stage.
 */

static char *str    = "hello";
static int  integer = 42;

module_param(integer, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(integer, "An integer with a very special meaning");

module_param(str, charp, 0000);
MODULE_PARM_DESC(str, "A character string");

```
```bash
insmod hello.ko str="hello world" integer=42
```

### Watch Kernel logs

```bash
dmesg -w
```

### Recommended Kernel options

```
MODULE_FORCE_UNLOAD
```
