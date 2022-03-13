#include <linux/init.h>
#include <linux/module.h>

static int __init	margin_init(void)
{
	printk(KERN_NOTICE "---------\n");
	return (0);
}

static void __exit margin_exit(void)
{
}

module_init(margin_init);
module_exit(margin_exit);

