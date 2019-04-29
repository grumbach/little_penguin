#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h>       /* needed for raw copy */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp,
			 char __user *user,
			 size_t size,
			 loff_t *offs);
static ssize_t myfd_write(struct file *fp,
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
	const size_t	len = strlen(str) + 1;

	if (user == NULL)
		goto fail;
	if (size > len)
		size = len;
	if (raw_copy_to_user(user, str, size))
		goto copyfail;

	return size;
fail:
	return -EINVAL;
copyfail:
	return -1;
}

static ssize_t myfd_write(struct file *fp,
			  const char __user *user,
			  size_t size,
			  loff_t *offs)
{
	if (user == NULL)
		goto fail;
	if (size >= PAGE_SIZE)
		size = PAGE_SIZE - 1;
	if (raw_copy_from_user(str, user, size))
		goto copyfail;

	str[size] = '\0';
	return size;
fail:
	return -EINVAL;
copyfail:
	return -1;
}

module_init(myfd_init);
module_exit(myfd_cleanup);
