#include <linux/module.h>
#include <linux/init.h>

static int my_id;

static int	get_my_id(void);
static int	set_my_id(int my_id);

EXPORT_SYMBOL(get_my_id);
EXPORT_SYMBOL(set_my_id);

static int	get_my_id(void)	
{
	return (my_id);
}

static int	set_my_id(int id)
{
	my_id = id;
	if (my_id == 201512285)
		return (1);
	else
		return (0);
}

static int __init	mod1_init(void)
{
	printk(KERN_NOTICE "Hello MOD1\n");
	return (0);
}

static void __exit	mod1_exit(void)
{
	printk(KERN_NOTICE "Bye MOD1\n");
}

module_init(mod1_init);
module_exit(mod1_exit);

