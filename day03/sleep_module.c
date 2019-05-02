#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>

static int      make_nap(int length)
{
	for (int i = 0; i < length; i++)
		udelay(10);
	if (length < 10)
		pr_info("We slept a long time!");

	return length * length;
}

static int      my_init(void)
{
	int     length = 10;

	length = make_nap(length);

	return length;
}

static void     my_exit(void)
{

}

module_init(my_init);
module_exit(my_exit);
