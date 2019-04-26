#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/fs.h>		/* Needed for I/O op */
#include <linux/miscdevice.h>	/* Needed for misc character device */
#include <asm/uaccess.h>

#define LOGIN "jye"
#define LOGIN_LEN 3

typedef struct miscdevice	mdev_t;
typedef struct file_operations	fops_t;

static mdev_t	_mdev;
static fops_t	_fops;

static ssize_t ft_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t ft_write(struct file *, const char __user *, size_t, loff_t *);

static mdev_t	_mdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fortytwo",
	.fops = &_fops,
};

static fops_t	_fops = {
	.owner = THIS_MODULE,
	.read = &ft_read,
	.write = &ft_write,
};

static ssize_t ft_read(struct file *filp, char __user *buf, size_t size, loff_t *pos)
{
	if (filp == (struct file *)0 || \
	    buf == (char *)0)
		return -1;
	if (size > LOGIN_LEN)
		size = LOGIN_LEN;
	pos += LOGIN_LEN;
	if (raw_copy_to_user(buf, LOGIN, LOGIN_LEN))
		return -1;
	return LOGIN_LEN;
}

static ssize_t ft_write(struct file *filp, const char __user *buf, size_t size, loff_t *pos)
{
	
	if (filp == (struct file *)0 || \
	    buf == (char *)0)
		return -1;
	if (memcmp(buf, LOGIN, LOGIN_LEN))
		return -1;
	pos += LOGIN_LEN;
	return LOGIN_LEN;
}

static int __init init_hello(void)
{
	int retval = -ENODEV;

	printk(KERN_NOTICE "Registering misc device fortytwo!\n");
	retval = misc_register(&_mdev);
	return retval;
}
module_init(init_hello);

static void __exit clean_hello(void)
{
	printk(KERN_NOTICE "Deregistering misc device fortytwo!\n");
	misc_deregister(&_mdev);
}
module_exit(clean_hello);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jye");
