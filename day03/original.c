#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>

static int do_work(int *my_len, int retval)
{
	int x;
	int y = *my_len;
	int z;
	
	for (x = 0; x < my_len; ++x)
		udelay(10);
	if(y < 10)
		pr_info("We slept a long time!");
	
	z = x * y;
	
	return z;
}

static int my_init(void)
{
	int x = 10;
	
	x = do_work(&x, x);
	
	return x;
}

static void my_exit(void)
{

}

module_init(my_init);
module_exit(my_exit);

