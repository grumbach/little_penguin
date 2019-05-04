#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/fs.h>		/* Needed for I/O op */
#include <linux/miscdevice.h>	/* Needed for misc character device */
#include <asm/uaccess.h>	/* For raw_copy_to_user */

#define LOGIN		"agrumbac"
#define LOGIN_LEN	sizeof(LOGIN)

static struct miscdevice _mdev;
static struct file_operations _fops;

static ssize_t ft_read(struct file *filp,
		       char __user *buf,
		       size_t size,
		       loff_t *pos);
static ssize_t ft_write(struct file *filp,
		        const char __user *buf,
		        size_t size,
		        loff_t *pos);

static struct miscdevice _mdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fortytwo",
	.fops = &ft_fops,
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
	loff_t porign = *ppos;

	if (buf == (char *)0 || size > LOGIN_LEN)
		goto einval;

	res = simple_write_to_buffer(id_buffer, LOGIN_LEN, ppos, buf, size);
	if (res > 0) {
		if (memcmp(id_buffer + porign, LOGIN + porign, res) == 0)
			goto success;
		else
			goto einval;
	}
einval:
	*ppos = 0;
	return -EINVAL;
success:
	if (*ppos >= LOGIN_LEN)
		*ppos = 0;
	return res;
}

static int __init init_hello(void)
{
	printk(KERN_NOTICE "Registering misc device fortytwo!\n");
	return misc_register(&ft_misc_device);
}
module_init(init_hello);

static void __exit clean_hello(void)
{
	printk(KERN_NOTICE "Deregistering misc device fortytwo!\n");
	misc_deregister(&ft_misc_device);
}
module_exit(clean_hello);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("agrumbac");
MODULE_DESCRIPTION("A misc device for fourtytwo!");
