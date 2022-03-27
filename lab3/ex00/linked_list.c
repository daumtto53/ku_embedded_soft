#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/kernel.h>
#include <linux/slab.h>

struct test_list
{
	struct	list_head list;
	int	id;
};

struct test_list	my_list;

static int __init	simple_linked_list_init(void)
{
	struct test_list	*tmp;
	struct list_head	*pos;
	unsigned int		i;

	printk("simple_linked_list : Init Module\n");

	INIT_LIST_HEAD(&my_list.list);

	for (i = 0; i < 5; i++)
	{
		tmp = (struct test_list *)kmalloc(sizeof(struct test_list), GFP_KERNEL);
		tmp->id = i;
		printk("simple_linked_list : enter to list [%d]\n", tmp->id);
		list_add(&(tmp->list), &(my_list.list));
	}

	i = 0;

	printk("simple_linked_list : use list for_each & list_entry\n");

	list_for_each(pos, &my_list.list)
	{
		tmp = list_entry(pos, struct test_list, list);
		printk("simple_linked_list : pos[%d], id[%d] \n", i, tmp->id);
		i++;
	}

	i = 0;

	printk("simple_linked_list : use list for_each_entry\n");
	list_for_each_entry(tmp, &my_list.list, list)
	{
		printk("simple_linked_list : pos[%d], id[%d] \n", i, tmp->id);
		i++;
	}
	return (0);
}

static void __exit	simple_linked_list_exit(void)
{
	struct test_list	*tmp = 0;
	struct list_head	*pos = 0;
	struct list_head	*q = 0;

	unsigned int i = 0;

	printk("simple_linked_list : Exit module\n");
	i = 0;
	list_for_each_safe(pos, q, &my_list.list)
	{
		tmp = list_entry(pos, struct test_list, list);
		printk("simple_linked_list : free pos[%d], id[%d]\n", i, tmp->id);
		list_del(pos);
		kfree(tmp);
		i++;
	}
}

module_init(simple_linked_list_init);
module_exit(simple_linked_list_exit);
