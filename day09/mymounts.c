#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/mount.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/nsproxy.h>
#include <../fs/mount.h>
#include <asm/uaccess.h>       /* needed for raw copy */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jye");
MODULE_DESCRIPTION("This module lists mount points on your system");

#define PROCFS_NAME 		"mymounts"

#define maybe_xkrealloc(needed, ptr, label)				\
	if ((needed) > proc_allocsize) {				\
		xkrealloc();						\
		if (_proc_state)					\
			goto label;					\
		ptr = memmove(proc_buffer - proc_len, (ptr), proc_len);	\
	}

static ssize_t _proc_read(struct file *, char __user *, size_t, loff_t *);
static int _proc_release(struct inode *, struct file *);
static int _proc_open(struct inode *, struct file *);

static struct file_operations p_fops = {
	.read = _proc_read,
	.open = _proc_open,
	.release = _proc_release,
	.owner = THIS_MODULE,
};

static struct proc_dir_entry *pdent;

static char __kernel *proc_buffer;
static size_t proc_len;
static size_t proc_allocsize;

static int _proc_lock;
static int _proc_state;

/*
 * moves from child to parents appending every path he founds
*/

static void xkrealloc(void)
{
	void *tmp = krealloc(proc_buffer, PAGE_SIZE + proc_allocsize, GFP_KERNEL);
	
	_proc_state = -(NULL == tmp);

	proc_buffer = tmp;
	proc_allocsize += PAGE_SIZE;
}

static char *reverse_traversal(struct dentry *dir)
{
	struct dentry *parent = (struct dentry *)0;
	size_t dname_len;
	const char *dname;
	char __kernel *ptr = proc_buffer + proc_allocsize - proc_len;

	for (; parent != dir->d_parent; parent = dir->d_parent, dir = dir->d_parent) {
		dname = dir->d_name.name;
		dname_len = strlen(dname) + 1;

		maybe_xkrealloc(dname_len + proc_len, ptr, fail);

		ptr -= dname_len;
		memcpy(&ptr[1], dname, dname_len - 1);

		if (dir->d_parent != dir) {
			ptr[0] = '/';
		} else {
			dname_len--;
			ptr++;
		}
		proc_len += dname_len;
	}
	return ptr;
fail:
	return (char *)0;
}


static void	prepare_output(struct mount *mnt)
{
	struct mountpoint *mnt_mp = mnt->mnt_mp;
	const char *devname = mnt->mnt_devname;
	size_t devname_len = strlen(devname);
	char *ptr;
	
	if (mnt_mp == (struct mountpoint *)0)
		goto end;

	ptr = reverse_traversal(mnt_mp->m_dentry);
	if (_proc_state)
		goto end;

	maybe_xkrealloc(devname_len + proc_len + 1, ptr, end);

	ptr[-1] = ' ';
	ptr = memcpy(ptr - devname_len - 1, devname, devname_len);
	proc_len += devname_len + 2;
	ptr[-1] = '\n';
end:
	return ;
}

static ssize_t _proc_read(struct file *filp,
			  char __user *buf,
			  size_t size,
			  loff_t *pos)
{
	if (_proc_state)
		goto critical;

	return simple_read_from_buffer(buf, size, pos, proc_buffer + proc_allocsize - proc_len, proc_len);
critical:
	return -ENOMEM;
}


/*
 * Prepare mymounts buffer, in reverse order because too lazy
*/

static int _proc_open(struct inode *inodep, struct file *filp)
{
	const size_t psize = sizeof(char) * PAGE_SIZE;
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	struct mount *mnt;

	if (_proc_lock)
		goto ebusy;

	proc_buffer = kmalloc(psize, GFP_KERNEL);
	if (proc_buffer == (char *)0)
		goto enomem;

	proc_allocsize = psize;
	proc_buffer[proc_allocsize - 1] = '\n';
	proc_len = 1;
	_proc_lock = 1;
	_proc_state = 0;

	list_for_each_entry(mnt, &ns->list, mnt_list) {
		prepare_output(mnt);

		if (_proc_state)
			goto enomem;
	}

	proc_len -= 1;

	return 0;
ebusy:
	return -EBUSY;
enomem:
	return -ENOMEM;
}

static int _proc_release(struct inode *inodep, struct file *filp)
{
	kfree(proc_buffer);
	proc_buffer = (char *)0;
	proc_allocsize = 0;
	_proc_lock = 0;
	return 0;
}

static int __init _proc_init(void)
{
	pdent = proc_create(PROCFS_NAME, 0444, NULL, &p_fops);
	if (pdent == (struct proc_dir_entry *)0)
		goto fail_alloc;

	return 0;
fail_alloc:
	remove_proc_entry(PROCFS_NAME, NULL);
	return -ENOMEM;
}

static void __exit _proc_cleanup(void)
{
	remove_proc_entry(PROCFS_NAME, NULL);
}

module_init(_proc_init);
module_exit(_proc_cleanup);
