#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/list.h>
#include <linux/slab.h>

#define KU_IPC(str)		printk("KU_IPC : %s\n", str)

MODULE_LICENSE("GPL");

int		__init my_init(void)
{
	KU_IPC("my_init");
	return (0);
}

void	__exit my_exit(void)
{
	KU_IPC("my_exit");
}

module_init(my_init);
module_exit(my_exit);
