#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/mount.h>
#include <asm/uaccess.h>       /* needed for raw copy */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("agrumbac");
MODULE_DESCRIPTION("This module lists mount points on your system");

static int __init myfd_init(void)
{
	struct dentry	*curdentry;

	list_for_each_entry(curdentry, &current->fs->root.mnt->mnt_root->d_subdirs, d_child)
	{
		if (curdentry->d_flags & DCACHE_MOUNTED)
			printk(KERN_INFO "%s", curdentry->d_name.name);
	}

	return 0;
}

static void __exit myfd_cleanup(void)
{

}

module_init(myfd_init);
module_exit(myfd_cleanup);
