#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("...");

static ssize_t myfd_read(structfile *fp,
			 char __user *user,
			 size_t size,
			 loff_t *offs);
static ssize_t myfd_write(structfile *fp,
			  const char __user *user,
			  size_t size,
			  loff_t *offs);

static char str[PAGE_SIZE];

static struct file_operations	myfd_fops = {
	.owner = THIS_MODULE,
	.read = &myfd_read,
	.write = &myfd_write
};

static struct miscdevice	myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops
};

static int __init myfd_init(void)
{
	int retval;

	retval = misc_register(&myfd_device);
	return retval;
}

static void __exit myfd_cleanup(void)
{
	misc_deregister(&myfd_device);
}

static ssize_t myfd_read(struct file *fp,
			 char __user *user,
			 size_t size,
			 loff_t *offs)
{
	
	return size;
fail:
	return -1;
}

static size_t myfd_write(structfile *fp,
			 const char __user *user,
			 size_t size,
			 loff_t *offs)
{
	return size;
fail:
	return -1;
}

module_init(myfd_init);
module_exit(myfd_cleanup);
