#include <linux/module.h>
#include <linux/init.h>

extern int my_id;

static int __init	mod2_init(void)
{
	printk(KERN_NOTICE "Hello MOD2\n");
	printk(KERN_NOTICE "My ID : %d\n", my_id);
	return (0);
}

static void __exit	mod2_exit(void)
{
	printk(KERN_NOTICE "Bye MOD2\n");
}

module_init(mod2_init);
module_exit(mod2_exit);
