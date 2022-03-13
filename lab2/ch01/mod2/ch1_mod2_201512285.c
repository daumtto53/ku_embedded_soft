#include <linux/module.h>
#include <linux/init.h>

extern int	get_my_id(void);
extern int	set_my_id(int id);

static int __init	ch1_mod2_init(void)
{
	printk(KERN_NOTICE "Hello MOD2\n");
	printk(KERN_NOTICE "My Id : %d\n", get_my_id());
	set_my_id(201512285);
	printk(KERN_NOTICE "My Id : %d\n", get_my_id());
	return (0);
}

static void __exit	ch1_mod2_exit(void)
{
	printk(KERN_NOTICE "Bye MOD2\n");
}

module_init(ch1_mod2_init);
module_exit(ch1_mod2_exit);






