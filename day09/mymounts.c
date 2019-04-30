#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/mount.h>
#include <linux/proc_fs.h>

#define PROCFS_NAME	"mymounts"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("agrumbac");
MODULE_DESCRIPTION("This module lists mount points on your system");

static ssize_t	procfile_read(struct file *filename,
			      char __user *buffer,
			      size_t buffer_length,
			      loff_t *off);

static struct proc_dir_entry		*proc_file_entry;
static const struct file_operations	mymounts_proc_fops = {
	.owner = THIS_MODULE,
	.read = procfile_read,
};
static int				are_we_empty = true;

static void	push_entry_to_buffer(char **buffer,
				     struct dentry *curdentry,
				     const char *mount_point)
{
	const char      *entry_name = \
		(mount_point ? (char *)curdentry->d_name.name : "root");
	const size_t    entry_len = strlen(entry_name);

	//add secu
	memcpy(*buffer, entry_name, entry_len);
	*buffer += entry_len;
	memcpy(*buffer, "\t", 1);
	*buffer += 1;
	if (mount_point)
	{
		const size_t	mount_len = strlen(mount_point);
		memcpy(*buffer, mount_point, mount_len);
		*buffer += mount_len;
		memcpy(*buffer, entry_name, entry_len);
		*buffer += entry_len;
	} else {	
		memcpy(*buffer, "/", 1);
		*buffer += 1;
	}
	memcpy(*buffer, "\n", 1);
	*buffer += 1;
}	

static ssize_t	procfile_read(struct file *filename,
			      char __user *buffer,
			      size_t buffer_length,
			      loff_t *off)
{
	const char	*buffer_start = buffer;
	struct dentry	*curdentry;

	printk(KERN_INFO "procfile_read (/proc/%s) called\n", PROCFS_NAME);
	are_we_empty ^= 0x1;

	if (are_we_empty == true)
		goto empty;

	curdentry = current->fs->root.mnt->mnt_root;
	push_entry_to_buffer(&buffer, curdentry, NULL);	

	list_for_each_entry(curdentry, &current->fs->root.mnt->mnt_root->d_subdirs, d_child)
	{
		if (curdentry->d_flags & DCACHE_MOUNTED)
			push_entry_to_buffer(&buffer, curdentry, "/");	
	}
	*buffer = '\0';
	return buffer - buffer_start;

empty:
	return 0;
}

static int __init myfd_init(void)
{
	proc_file_entry = proc_create(PROCFS_NAME, 0444, NULL, &mymounts_proc_fops);

	if(proc_file_entry == NULL)
		goto fail;

	printk(KERN_INFO "/proc/%s created\n", PROCFS_NAME);
	return 0;

fail:
	remove_proc_entry(PROCFS_NAME, NULL);
	printk(KERN_ALERT "Error: Could not initialize /proc/%s\n", PROCFS_NAME);
	return -ENOMEM;
}

static void __exit myfd_cleanup(void)
{
	remove_proc_entry(PROCFS_NAME, NULL);
}

module_init(myfd_init);
module_exit(myfd_cleanup);
