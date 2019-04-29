#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/debugfs.h>	/* Needed for debugfs */
#include <linux/jiffies.h>	/* Needed for jiffies */
#include <linux/fs.h>		/* Needed for fileop */
#include <linux/slab.h>		/* Needed for kernel allocation */
#include <linux/gfp.h>		/* Needed for allocation flags */
#include <asm/uaccess.h>

#define DENTRY_NULL ((dentry_t *)0)

#define LOGIN "jye"
#define LOGIN_LEN 3

typedef struct dentry dentry_t;
typedef struct file_operations fops_t;

/* directory debugfs */
static dentry_t *dentry;
/* file debugfs */
#ifndef __CIVILLIZED_TIME
static dentry_t *_jiffies;
static dentry_t *foo;
static dentry_t *id;
#endif

static ssize_t jif_read(struct file *, char __user *, size_t, loff_t *);

static ssize_t foo_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t foo_read(struct file *, char __user *, size_t, loff_t *);

static ssize_t id_write(struct file *,const char __user *, size_t, loff_t *);
static ssize_t id_read(struct file *, char __user *, size_t, loff_t *);

static fops_t jif_ops = {
	.owner = THIS_MODULE,
	.read = jif_read,
};

static fops_t foo_ops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write,
};

static fops_t id_ops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write,
};

static u8 *fookbuf;
static u64 fookbuf_size;
static u64 fookbuf_used;

static DEFINE_SPINLOCK(x_lock);

static ssize_t foo_read(						\
	struct file *filp, char __user *buf, size_t size, loff_t *pos)
{
	if (filp == (struct file *)0 ||		\
	    buf == (char *)0)
		return -1;
	spin_lock(&x_lock);
	if (fookbuf_used < size)
		size = fookbuf_used;
	if (size == 0)
		goto end;
	if (raw_copy_to_user(buf, fookbuf, size))
		return -1;
	pos += size;
end:
	spin_unlock(&x_lock);
	return size;
}

static ssize_t foo_write(						\
	struct file *filp, const char __user *buf, size_t size, loff_t *pos)
{
	const ssize_t _avail = (fookbuf_size - fookbuf_used);

	if (filp == (struct file *)0 ||		\
	    buf == (char *)0)
		return -1;
	spin_lock(&x_lock);
	if (_avail < size)
		size = _avail;
	if (size == 0)
		goto end;
	if (raw_copy_from_user(fookbuf + fookbuf_used, buf, size))
		return -1;
	fookbuf_used += size;
	pos += size;
end:
	spin_unlock(&x_lock);
	return size;
}

static ssize_t jif_read(						\
	struct file *filp, char __user *buf, size_t size, loff_t *pos)
{
	u64 _jif;

	if (filp == (struct file *)0 ||		\
	    buf == (char *)0)
		return -1;
	if (size > sizeof(u64))
		size = sizeof(u64);
	_jif = get_jiffies_64();
	if (raw_copy_to_user(buf, &_jif, size))
		return -1;
	return size;
}

static ssize_t id_read(							\
	struct file *filp, char __user *buf, size_t size, loff_t *pos)
{
	if (filp == (struct file *)0 ||		\
	    buf == (char *)0)
		return -1;
	if (size > LOGIN_LEN)
		size = LOGIN_LEN;
	pos += LOGIN_LEN;
	if (raw_copy_to_user(buf, LOGIN, LOGIN_LEN))
		return -1;
	return LOGIN_LEN;
}

static ssize_t id_write(						\
	struct file *filp, const char __user *buf, size_t size, loff_t *pos)
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
#ifdef __CIVILIZED_TIME
	dentry_t *_jiffies;
	dentry_t *foo;
	dentry_t *id;
#endif

	dentry = debugfs_create_dir("fortytwo", DENTRY_NULL);
	if (dentry == DENTRY_NULL || dentry == ERR_PTR(-ENODEV))
		return -ENODEV;
	fookbuf_size = PAGE_SIZE;
	fookbuf = kmalloc(fookbuf_size, GFP_KERNEL);
	if (fookbuf == (void *)0)
		goto fail_alloc;
	_jiffies = debugfs_create_file("jiffies", 0444, dentry, 0, &jif_ops);
	foo = debugfs_create_file("foo", 0644, dentry, 0, &foo_ops);
	id = debugfs_create_file("id", 0666, dentry, 0, &id_ops);
	if (_jiffies == DENTRY_NULL ||		\
	    id == DENTRY_NULL ||		\
	    foo == DENTRY_NULL)
		goto fail_file;
	return 0;

fail_file:
	kfree(fookbuf);
	debugfs_remove_recursive(dentry);
fail_alloc:
	debugfs_remove(dentry);
	return -1;
}
module_init(init_hello);

static void __exit clean_hello(void)
{
#ifdef __CIVILIZED_TIME
	debugfs_remove_recursive(dentry);
#else
	debugfs_remove(_jiffies);
	debugfs_remove(foo);
	debugfs_remove(id);
#endif
	debugfs_remove(dentry);
	kfree(fookbuf);
	printk(KERN_NOTICE "Cleaning up module.\n");
}
module_exit(clean_hello);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jye");
MODULE_DESCRIPTION("some virtual file system");
