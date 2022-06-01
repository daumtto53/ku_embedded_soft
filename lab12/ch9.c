#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

#define BUF_SIZE 50
static struct proc_dir_entry *parent_proc;
static char name[100] = "";
static int count = 0;

static int proc_info_show(struct seq_file *seq, void *v)
{
	seq_printf(seq, "%s\n\n", name);
	seq_printf(seq, "str : %d\n", count);

	return (0);
}

static int proc_info_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_info_show, NULL);
}

static ssize_t proc_info_write(struct file *file, const char __user *ubuf, size_t ubuf_len, loff_t *pos)
{
	char buf[BUF_SIZE];
	int i = 0;
	char	t;

	if (ubuf_len > BUF_SIZE)
		return (-1);
	if (copy_from_user(buf, ubuf, ubuf_len))
		return (-1);

	memset(name , 0 , sizeof(name));
	sscanf(buf, "%s", name);
	count = 0;

	printk("ch9 str : %s\n", name);
	t = 0;
	while (buf[i])
	{
		if (buf[i] == ' ')
			count++;
		i++;
	}
	printk("count : %d\n", count);


	return strlen(buf);
}

static const struct file_operations proc_info_fops = {
	.open = proc_info_open,
	.read = seq_read,
	.write = proc_info_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init simple_proc_init(void)
{
	proc_create("ch9", 0666, NULL, &proc_info_fops);
	return (0);
}

static void __exit simple_proc_exit(void)
{
	proc_remove(parent_proc);
}

module_init(simple_proc_init);
module_exit(simple_proc_exit);
