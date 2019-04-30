#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/debugfs.h>	/* Needed for debugfs */
#include <linux/jiffies.h>	/* Needed for jiffies */
#include <linux/fs.h>		/* Needed for fileop */
#include <linux/slab.h>		/* Needed for kernel allocation */
#include <linux/gfp.h>		/* Needed for allocation flags */
#include <asm/uaccess.h>	/* For raw_copy */

#define LOGIN		"agrumbac"
#define LOGIN_LEN	sizeof(LOGIN)

static struct dentry	*debug_dir;

/*
 * callback prototypes
 */

static ssize_t foo_read(struct file *filp,
			char __user *buf,
			size_t size,
			loff_t *pos);
static ssize_t foo_write(struct file *filp,
			 const char __user *buf,
			 size_t size,
			 loff_t *pos);
static ssize_t jif_read(struct file *filp,
			char __user *buf,
			size_t size,
			loff_t *pos);
static ssize_t id_read(struct file *filp,
		       char __user *buf,
		       size_t size,
		       loff_t *pos);
static ssize_t id_write(struct file *filp,
			const char __user *buf,
			size_t size,
			loff_t *pos);
/*
 * ops structs
 */

static struct file_operations	jif_ops = {
	.owner = THIS_MODULE,
	.read = jif_read,
};

static struct file_operations	foo_ops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write,
};

static struct file_operations	id_ops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write,
};

static u8			*foo_kbuf;
static u64			foo_kbuf_size = PAGE_SIZE;
static u64			foo_kbuf_used;

static DEFINE_SPINLOCK(spinlock);

static ssize_t foo_read(struct file *filp,
			char __user *buf,
			size_t size,
			loff_t *pos)
{
	if (filp == NULL || buf == NULL)
		return -1;
	spin_lock(&spinlock);
	if (foo_kbuf_used < size)
		size = foo_kbuf_used;
	if (size == 0)
		goto end;
	if (raw_copy_to_user(buf, foo_kbuf, size))
		return -1;
end:
	spin_unlock(&spinlock);
	return size;
}

static ssize_t	foo_write(struct file *filp,
			  const char __user *buf,
			  size_t size,
			  loff_t *pos)
{
	const ssize_t available_size = (foo_kbuf_size - foo_kbuf_used);

	if (filp == NULL || buf == NULL)
		return -1;
	spin_lock(&spinlock);
	if (available_size < size)
		size = available_size;
	if (size == 0)
		goto end;
	if (raw_copy_from_user(foo_kbuf + foo_kbuf_used, buf, size))
		return -1;
	foo_kbuf_used += size;
end:
	spin_unlock(&spinlock);
	return size;
}

static ssize_t	jif_read(struct file *filp,
			 char __user *buf,
			 size_t size,
			 loff_t *pos)
{
	u64	jiffies;

	if (filp == NULL || buf == NULL)
		return -1;
	if (size > sizeof(u64))
		size = sizeof(u64);
	jiffies = get_jiffies_64();
	if (raw_copy_to_user(buf, &jiffies, size))
		return -1;
	return size;
}

static ssize_t	id_read(struct file *filp,
			char __user *buf,
			size_t size,
			loff_t *pos)
{
	if (filp == NULL || buf == NULL)
		return -1;
	if (size > LOGIN_LEN)
		size = LOGIN_LEN;
	if (raw_copy_to_user(buf, LOGIN, LOGIN_LEN))
		return -1;
	return LOGIN_LEN;
}

static ssize_t	id_write(struct file *filp,
			 const char __user *buf,
			 size_t size,
			 loff_t *pos)
{
	if (filp == NULL || buf == NULL)
		return -1;
	if (memcmp(buf, LOGIN, LOGIN_LEN))
		return -1;
	return LOGIN_LEN;
}

static int __init	init_debugfs_module(void)
{
	struct dentry	*jiffies_file;
	struct dentry	*foo_file;
	struct dentry	*id_file;

	debug_dir = debugfs_create_dir("fortytwo", NULL);
	if (debug_dir == NULL || debug_dir == ERR_PTR(-ENODEV))
		return -ENODEV;
	foo_kbuf = kmalloc(foo_kbuf_size, GFP_KERNEL);
	if (foo_kbuf == NULL)
		goto fail_alloc;

	jiffies_file = debugfs_create_file("jiffies", 0444, debug_dir, 0, &jif_ops);
	foo_file = debugfs_create_file("foo", 0644, debug_dir, 0, &foo_ops);
	id_file = debugfs_create_file("id", 0666, debug_dir, 0, &id_ops);
	if (jiffies_file == NULL || foo_file == NULL || id_file == NULL)
		goto fail_file;

	return 0;

fail_file:
	kfree(foo_kbuf);
	debugfs_remove_recursive(debug_dir);
fail_alloc:
	debugfs_remove(debug_dir);
	return -1;
}

static void __exit exit_debugfs_module(void)
{
	debugfs_remove_recursive(debug_dir);
	debugfs_remove(debug_dir);
	kfree(foo_kbuf);
	printk(KERN_NOTICE "Cleaning up module.\n");
}

module_init(init_debugfs_module);
module_exit(exit_debugfs_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("agrumbac");
MODULE_DESCRIPTION("some virtual file system");
