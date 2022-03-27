# lab_2 변수,함수,구조체 관련

## cdev 구조체

```c
#include/linux/cdev.h


struct cdev
{
    struct kobject kobj;    
    struct module *owner;
    const struct file_operations *ops;        // 조작해줄건 여기거.
    struct list_head list;
    dev_t    dev;
    unsigned int count;
}
```

  

## file_operations *ops

```c
struct file_operations {
    struct module *owner;
    loff_t (*llseek) (struct file *, loff_t, int);
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
    // ...

    __poll_t (*poll) (struct file *, struct poll_table_struct *);
    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);

    int (*open) (struct inode *, struct file *);
    int (*flush) (struct file *, fl_owner_t id);
    int (*release) (struct inode *, struct file *);
    int (*fsync) (struct file *, loff_t, loff_t, int datasync);
    int (*fasync) (int, struct file *, int);
    int (*lock) (struct file *, int, struct file_lock *);

    // ...

#ifndef CONFIG_MMU
    unsigned (*mmap_capabilities)(struct file *);
#endif
    ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
            loff_t, size_t, unsigned int);
    loff_t (*remap_file_range)(struct file *file_in, loff_t pos_in,
                   struct file *file_out, loff_t pos_out,
                   loff_t len, unsigned int remap_flags);
} __randomize_layout;
```

## various file_operations

```c
    int (*open) (struct inode *, struct file *);
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
    __poll_t (*poll) (struct file *, struct poll_table_struct *);
    int (*release) (struct inode *, struct file *);
    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
```

> [Character device drivers &mdash; The Linux Kernel documentation](https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html)

### open

```c
static int my_open(struct inode *inode, struct file *file)
{
    struct my_device_data *my_data =
             container_of(inode->i_cdev, struct my_device_data, cdev);

    /* validate access to device */
    file->private_data = my_data;

    /* initialize device */
    ...

    return 0;
}
```

### read

```c
static int my_read(struct file *file, char __user *user_buffer,
                   size_t size, loff_t *offset)
{
    struct my_device_data *my_data = (struct my_device_data *) file->private_data;
    ssize_t len = min(my_data->size - *offset, size);

    if (len <= 0)
        return 0;

    /* read data from my_data->buffer to user buffer */
    if (copy_to_user(user_buffer, my_data->buffer + *offset, len))
        return -EFAULT;

    *offset += len;
    return len;
}
```

### cdev_add(struct cdev *dev, dev_t num, unsigned int count)

* dev : cdev 구조체

* num : `alloc_chrdev_region` 에서 할당된 major + minor Number를 고려하여 계산된 값이 들어감.

* count : 이 device에 연결될 device driver의 minor number 개수..?

### alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name)

* dev : output으로 받을 parameter. (minor + major number)

* basaeminor :  minor number 의 시작 주소.  minor number가 시작하는 번호인듯..?

* count : minor number 개수.

* name : device driver의 이름.

### Character Device init 시 cdev 관련 함수들

```
cd_cdev = cdev_alloc();
cdev_init(cd_cdev, &simple_char_fops());
cdev_del(cd_cdev);
unregister_chrdev_region(dev_num, 1);
```

## awk

```shell
#! /bin/bash


MODULE="mymod"
major=${awk "\$2==$MODULE\" {print \$1}" /proc/devices}
mknod /dev/${module}0 c $major 0        #(minor number) 0
```

## parameter passing

```c
static int var = 0;
module_param(count, int, S_IRUGO);
```

# Character Device 만들어보기

## Character Device Driver 작성해보기

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>        //file_operations structure
#include <linux/cdev.h>      //cdev structure, functions

#define DEV_NAME "simple_char_dev" //ㅇㄷ

//fs.h의 file_operations에 있는 프로토타입 그대로 작성.
static int simple_char_open(struct inode *inode, struct file *file)
{
    printk("simple char : open()\n");
    return (0);
}

static int simple_char_release(struct inode *inode, struct file *file)
{
    printk("simple_char : release()\n");
    return (0);
}

static ssize_t simple_char_read(struct file *file, char *buf, size_t len, loff_t *lof)
{
    printk("simple_char : read()\n");
    return (0);
}

//write 의 parameter인 buf 는 const char *로 들어가야 에러가 안남.
static ssize_t simple_char_write(struct file *file, const char *buf, size_t len, loff_t *lof)
{
    printk("simple_char : write %s\n", buf);
    return (len);
}

// 하나의 문법인것. fops 구조체를 초기화해주기 위한.
struct file_operations simple_char_fops = {
    .open = simple_char_open,
    .release = simple_char_release,
    .read = simple_char_read,
    .write = simple_char_write,
}

static dev_t    dev_num;
static struct cdev    *cd_cdev;

static int __init simple_char_init(void)
{
    printk("simple_char : init_module\n");

    alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);   //device file 이름을 제대로 명시해주지 않으면 실행이 안됨.
    cd_cdev = cdev_alloc();
    cdev_init(cd_cdev, &simple_char_fops());
    cdev_add(cd_cdev, dev_num, 1);

    return (0);
}

static void __exit simple_char_exit(void)
{
    printk("simple_char : exit_module\n");

    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);
}

module_init(simple_char_init);
module_exit(simple_char_exit);
```

1. file_ops 의 operation 정의

2. file_ops의 file_operation 구조체 정의

3. `__init` 에서 magic number 할당 -> cdev 구조체 할당 -> cdev 구조체에 ops 연결 -> cdev_add()

4. `__exit` 에서 cdev_del, magic number 할당 해제

## Character Device File 만들기

```c
#! /bin/sh

MODULE="simple_char_dev"
MAJOR=$(awk "\$2==\"MODULE\" {print \$1}" /proc/devices)

mknod /dev/$MODULE0 c $MAJOR 0
```

## simple_char_app.c 작성

```c
#include <stdio.h>
#include <sys/fcntl.h>
#inlcude <unistd.h>
void main(void)
{
    int dev;
    dev=  open("/dev/simple_char_dev", O_RDWR);    
    write(dev, "HELLO!", 30);
    close(dev);
}
```

## 모듈 실행 과정

1. 필요한 파일 생성
   
   1. character device driver, device file, test file for testing chrdev driver, makefile, mknod.sh

2. make

3. insmod - 링크, 모듈 설치

4. mknod1

5. test file 실행

6. 커널 메시지 확인
   
   dmesg

7. 모듈 제거, device file 제거
   
   1. rmmod
   
   2. rm /dev/file

# IOCTL

`long (*unlocked ioctl) (struct file *file, unsigned cmd, unsigned long arg)`

* file : ioctl을 사용할 device file의 fd

* cmd :

* arg : 구조체의 주소 또는 인자값의 value

### cmd 구조 :

* Direction : IO Device를 위해서 읽기/쓰기 Direction을 정해줌.

* Size : 매개변수 데이터의 길이

* Magic Number : ?
  
  * IO Device의 CMD와 겹치지 않게 Magic Nubmer 잘 설정.
  
  * documentation/ioctl-number.txt 에 적혀져 있으니.. 알아서 잘 설정.

* Number : cmd 번호..?

### CMD 생성 매크로

```c
_IO(Magic number, number)
_IOR(Magic Number, number, arg length)
_IOW(Magic Number, number, arg length)
_IOWR(Magic Number, number, arg length)
```

### 예제

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define DEV_NAME "simple_iotcl_dev"

#define SIMPLE_IOCTL_NUM 0x80       //IOCTL_NUM : Command 번호
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2

#define SIMPLE_IOCTL_NUM    'z' //Magic Number
#define SIMPLE_IOCTL1    _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long)
#define SIMPLE_IOCTL2    _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)


static int simple_ioctl_open(struct inode *inode, struct file *file)
{
    printk("open\n");
    return (0);
}

static int simple_ioctl_release(struct inode *inode, struct file *file)
{
    printk("close\n");
    return (0);
}

static long simple_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

    switch (cmd)
    {
        case SIMPLE_IOCTL1:    //SIMPLE_IOCTL1 매크로가 cmd를 계산해줌.
            printk("simple_ioctl : IOCTL1 : %lu\n", arg);
            break;
        case SIMPLE_IOCTL2 :    //SIMPLE_IOCTL2 매크로가 cmd를 계산해줌.
            printk("simple_ioctl : IOCTL2 : %p\n", arg);
            break;
        default :
            return (-1);
    }
    return (0);
}

struct file_operation simple_ioctl_fops = {
    .open = simple_ioctl_open,
    .release = simple_ioctl_release,
    .unlocked_ioctl = simple_ioctl,
}

static dev_t    dev_num;
static struct cdev    *cd_cdev;

static int __init simple_char_init(void)
{
    printk("simple_char : init_module\n");

    alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
    cd_cdev = cdev_alloc();
    cdev_init(cd_cdev, &simple_char_fops());
    cdev_add(cd_cdev, dev_num, 1);

    return (0);
}

static void __exit simple_char_exit(void)
{
    printk("simple_char : exit_module\n");

    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);
}

module_init(simple_char_init);
module_exit(simple_char_exit);
```

```c
// SIMPLE_IOCTL_APP

#include <stdio.h>
#include <sys/fctnl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// IOCTL에서 사용할 매크로는 다 알고 있어야함.

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1    IOCTL_START_NUM+1
#define IOCTL_NUM2    IOCTL_START_NUM+2

#define SIMPLE_IOCTL_NUM 'z'
#define SIMPLE_IOCTL1    _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long)
#define SIMPLE_IOCTL2    _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)

void main(void)
{
    int fd;
    unsigned long value = 200;

    fd = open("dev/simple_ioctl_dev". O_RDWR);

    ioctl(fd, SIMPLE_IOCTL1, value);
    ioctl(fd, SIMPLE_IOCTL1, &value);

    close(fd);

}
```

### SYSCALL : ioctl(fd, cmd, arg)

# Parameter Passing

```c
static long my_id = 0;
module_param(my_id, long, 0);
```

# Makefile 작성 :

```makefile
...

default:
    $(MAKE) -C $(KERNELDIR) M=$(PWD) modules
    gcc -o $(obj-m:.o=_app) $(obj-m:.o=_app.c)
    ./mknod.sh
clean:
    $(MAKE) -C $(KERNELDIR) M=$(PWD) clean
    rm $(obj-m:.o=_app)
```
