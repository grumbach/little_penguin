#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/fs.h>		/* Needed for I/O op */
#include <linux/miscdevice.h>	/* Needed for misc character device */
#include <asm/uaccess.h>	/* For raw_copy_to_user */

#define LOGIN		"agrumbac"
#define LOGIN_LEN	sizeof(LOGIN)

typedef struct miscdevice	mdev_t;
typedef struct file_operations	fops_t;

static mdev_t			ft_misc_device;
static fops_t			ft_fops;

static ssize_t ft_read(struct file *filp,
		       char __user *buf,
		       size_t size,
		       loff_t *pos);
static ssize_t ft_write(struct file *filp,
		        const char __user *buf,
		        size_t size,
		        loff_t *pos);

static mdev_t	ft_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fortytwo",
	.fops = &ft_fops,
};

static fops_t	ft_fops = {
	.owner = THIS_MODULE,
	.read = &ft_read,
	.write = &ft_write,
};

static ssize_t ft_read(struct file *filp,
		       char __user *buf,
		       size_t size,
		       loff_t *pos)
{
	if (filp == NULL || buf == NULL)
		return -1;
	if (size > LOGIN_LEN)
		size = LOGIN_LEN;
	pos += LOGIN_LEN;
	if (raw_copy_to_user(buf, LOGIN, LOGIN_LEN))
		return -1;
	return LOGIN_LEN;
}

static ssize_t ft_write(struct file *filp,
			const char __user *buf,
			size_t size,
			loff_t *pos)
{
	if (filp == NULL || buf == NULL)
		return -1;
	if (memcmp(buf, LOGIN, LOGIN_LEN))
		return -1;
	pos += LOGIN_LEN;
	return LOGIN_LEN;
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
