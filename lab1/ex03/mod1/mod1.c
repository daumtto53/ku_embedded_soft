#include <linux/module.h>
#include <linux/init.h>

int	my_id = 201512285;
EXPORT_SYMBOL(my_id);

static int __init mod1_init(void)
{
	printk(KERN_NOTICE "Hello MOD1\n");
	return (0);
}

static void __exit mod1_exit(void)
{
	printk(KERN_NOTICE "Goodbye MOD1\n");
}

module_init(mod1_init);
module_exit(mod1_exit);
