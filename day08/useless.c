#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>
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
	ssize_t	t, i;
	ssize_t	ret;
	char *tmp;

	if (user == NULL)
		goto fail;

	tmp = kmalloc(sizeof(char) * PAGE_SIZE * 2, GFP_KERNEL);
	if (!tmp)
		goto enomem;

	for (t = strlen(str) - 1, i = 0; t >= 0; t--, i++)
		tmp[i] = str[t];

	ret = simple_read_from_buffer(user, size, offs, tmp, i);
	kfree(tmp);

	return ret;
fail:
	return -EINVAL;
enomem:
	return -ENOMEM;
}

static ssize_t myfd_write(struct file *fp,
			  const char __user *user,
			  size_t size,
			  loff_t *offs)
{
	ssize_t res;

	if (user == NULL)
		goto fail;

	res = simple_write_to_buffer(str, PAGE_SIZE - 1, offs, user, size) + 1;

	str[size] = '\0';
	return res;
fail:
	return -EINVAL;
}

module_init(myfd_init);
module_exit(myfd_cleanup);
