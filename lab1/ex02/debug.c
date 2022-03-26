#include <linux/module.h>
#include <linux/init.h>

#ifdef MYDEBUG
	#define DEBUGMSG(fmt, args...) printk(KERN_DEBUG "MYDEBUG : " fmt, ##args);
#else
	#define DEBUGMSG(fmt, args...)
#endif

static int __init	debug_init(void)
{
	printk(KERN_NOTICE "Debug init\n");
	DEBUGMSG("THIS IS DEBUGGING MESSGAGE");
	return (0);
}

static void __exit	debug_exit(void)
{
	printk(KERN_NOTICE "Debug exit\n");
}

module_init(debug_init);
module_exit(debug_exit);
