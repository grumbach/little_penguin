#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/fs.h>		/* Needed for I/O op */
#include <linux/miscdevice.h>	/* Needed for misc character device */
#include <asm/uaccess.h>

#define LOGIN "jye"
#define LOGIN_LEN 3

static struct miscdevice _mdev;
static struct file_operations _fops;

static ssize_t ft_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t ft_write(struct file *, const char __user *, size_t, loff_t *);

static struct miscdevice _mdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fortytwo",
	.fops = &_fops,
};

static struct file_operations _fops = {
	.owner = THIS_MODULE,
	.read = &ft_read,
	.write = &ft_write,
};

static char id_buffer[PAGE_SIZE];

static ssize_t ft_read(struct file *filp,	\
		       char __user *buf,	\
		       size_t size,		\
		       loff_t *ppos)
{
	ssize_t res;

	if (buf == (char *)0)
		goto einval;

	res = simple_read_from_buffer(buf, size, ppos, LOGIN, LOGIN_LEN);
	if (res == 0 && *ppos == 3)
		*ppos = 0;
	return res;
einval:
	return -EINVAL;
}

static ssize_t ft_write(struct file *filp,	\
			const char __user *buf,	\
			size_t size,		\
			loff_t *ppos)
{
	ssize_t res;

	if (buf == (char *)0 || size > LOGIN_LEN)
		goto einval;

	res = simple_write_to_buffer(id_buffer, LOGIN_LEN, ppos, buf, size);
	if (res > 0 && *ppos == 3) {
		*ppos = 0;
		if (memcmp(id_buffer, LOGIN, LOGIN_LEN) == 0)
			goto success;
		else
			goto einval;
	}
	return res;
einval:
	return -EINVAL;
success:
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
