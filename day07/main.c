#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/debugfs.h>	/* Needed for debugfs */
#include <linux/jiffies.h>	/* Needed for jiffies */
#include <linux/fs.h>		/* Needed for fileop */
#include <linux/slab.h>		/* Needed for kernel allocation */
#include <linux/gfp.h>		/* Needed for allocation flags */
#include <asm/uaccess.h>

#define DENTRY_NULL ((struct dentry *)0)

#define LOGIN "jye"
#define LOGIN_LEN 3


/* directory debugfs */
static struct dentry *dir;

static ssize_t foo_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t foo_read(struct file *, char __user *, size_t, loff_t *);

static ssize_t id_write(struct file *,const char __user *, size_t, loff_t *);
static ssize_t id_read(struct file *, char __user *, size_t, loff_t *);

static const struct file_operations foo_ops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write,
};

static const struct file_operations id_ops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write,
};

static char id_buffer[PAGE_SIZE];
static u8 fookbuf[PAGE_SIZE];
static u64 fookbuf_used;

static DEFINE_SPINLOCK(x_lock);

static ssize_t foo_read(struct file *filp,	\
			char __user *buf,	\
			size_t size,		\
			loff_t *ppos)
{
	ssize_t res;

	if (buf == (char *)0)
		goto einval;

	spin_lock(&x_lock);
	res = simple_read_from_buffer(buf, size, ppos, fookbuf, fookbuf_used);

	spin_unlock(&x_lock);
	return res;
einval:
	return -EINVAL;
}

static ssize_t foo_write(struct file *filp,		\
			 const char __user *buf,	\
			 size_t size,			\
			 loff_t *ppos)
{
	ssize_t res;

	if (buf == (char *)0)
		goto einval;

	spin_lock(&x_lock);
	res = simple_write_to_buffer(fookbuf, PAGE_SIZE, ppos, buf, size);

	if (res > 0)
		fookbuf_used += res;

	spin_unlock(&x_lock);
	return size;
einval:
	return -EINVAL;
}

static ssize_t id_read(struct file *filp,	\
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

static ssize_t id_write(struct file *filp,	\
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
	struct dentry *_jiffies;
	struct dentry *foo;
	struct dentry *id;

	dir = debugfs_create_dir("fortytwo", DENTRY_NULL);
	if (dir == DENTRY_NULL || dir == ERR_PTR(-ENODEV))
		goto enodev;

	_jiffies = debugfs_create_u64("jiffies", 0444, dir, &jiffies_64);
	foo = debugfs_create_file("foo", 0644, dir, 0, &foo_ops);
	id = debugfs_create_file("id", 0666, dir, 0, &id_ops);
	if (_jiffies == DENTRY_NULL ||		\
	    id == DENTRY_NULL ||		\
	    foo == DENTRY_NULL)
		goto fail_file;

	return 0;
enodev:
	return -ENODEV;
fail_file:
	debugfs_remove_recursive(dir);
	debugfs_remove(dir);
	return -ENOMEM;
}
module_init(init_hello);

static void __exit clean_hello(void)
{
	debugfs_remove_recursive(dir);
	debugfs_remove(dir);
}
module_exit(clean_hello);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jye");
MODULE_DESCRIPTION("some virtual file system");
