[toc]





## printk



```c
printk(KERN_INFO "Message: %s\n", arg);
```



printk(KERN_INFO, msg)





```shell
# dmesg -n 5
```





https://docs.kernel.org/kbuild/modules.html#targets

* Kernel log level - KERN_INFO Shell에서 변경.

* KBUILD_EXTRA_SYMBOLS

* > > - Use “make” variable KBUILD_EXTRA_SYMBOLS
  > >
  > >   If it is impractical to add a top-level kbuild file, you can assign a space separated list of files to KBUILD_EXTRA_SYMBOLS in your build file. These files will be loaded by modpost during the initialization of its symbol tables


  ```makefile
  KBUILD_EXTRA_SYMBOLS := symvers의 절대경로..

https://unix.stackexchange.com/questions/214313/how-to-change-the-console-loglevel-in-linux





## Makefile

> makefile 과 관련한 추가정보들 : https://docs.kernel.org/kbuild/makefiles.html

```makefile
obj-m := mymod.o
KERNELDIR := /lib/modules/$(shell uname –r)/build
PWD := $(shell pwd)
default:
$(MAKE) –C $(KERNELDIR) M=$(PWD) modules
clean:
$(MAKE) –C $(KERNELDIR) M=$(PWD) clean
  ```



## Makefile을 쓰는 이유 :

kbuild system을 사용해서 External module을 얹을 것이기 때문에.

 External Module을 빌드하기 위해서는 커널이 있어야하며, 빌드가 되어있어야한다. distro를 사용중이라면, package가 존재할 것.

> Kbuild란 ?
> 	리눅스 커널에서 사용하는 build system.



```shell
$ make -C <path_to_kernel_src> M=$PWD
```

를 사용해서 external module을 빌드 할 것.

`M=<DIR>` 를 사용해서 kbuild system에게 커널을 빌드하고 있다고 알린다.



## Options

> ($KDIR refers to the path of the kernel source directory.)
>
> make -C $KDIR M=$PWD
>
> - -C $KDIR
>
>   The directory where the kernel source is located. “make” will actually change to the specified directory when executing and will change back when finished.
>
> - M=$PWD
>
>   Informs kbuild that an external module is being built. The value given to “M” is the absolute path of the directory where the external module (kbuild file) is located.



## obj-m :

1. <module_name>.c

2.  module_name.o
3. (linking 이후) module_name.ko



#### kbuild system의 makefile의 정리

C option으로 kernel을 build하고, M option으로 external module을 같이 build하고 있읆을 알린다.



### 여러 파일 빌드하기

> ```makefile
> make -C $KDIR M=$PWD bar.lst
> make -C $KDIR M=$PWD baz.o
> make -C $KDIR M=$PWD foo.ko
> make -C $KDIR M=$PWD ./
> ```



### := 와 = 의 차이

`obj-m := mymod.o	`



:=  : Recursive assignment.

​	변수가 re-evaluated 되는 상황에서 쓰인다. 예를 들어, a 가 a = 123 이었다가 , 추후 Makefile에서의 흐름에서 a=456으로 바뀌는 상황이 있을 수 있으니, 변수에 assign된 것이 바뀌는 상황에서 쓰인다고 할 수 있다.



= : Simple assignment.

​	변수가 바뀌지 않는 상황에서 쓰인다.



Symbol Table에 올리냐, 올리지 않고 지속적으로 변화를 관찰하느냐의 상황에서 사용을 달리하는것같다.





### uname

```shell
$ uname -r
5.4.0-100-generic
$ uname -n
cms-desktop
$ uname -s
Linux
```









## daemon 이란?

> 사용자가 직접적으로 제어하지 않고, 백그라운드에서 돌면서 여러 작업을 하는 프로그램을 말한다





## Debugging in Kernel

https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=kimyoseob&logNo=220639377819

```c
#ifdef MY_DEBUG
		#define DEBUG_MSG(fmt, args…) \
	printk(KERN_DEBUG “MYDEBUG: “ fmt, ##args)
#else
	#define DEBUG_MSG(fmt, args…)
#endif
```



`printk("aaaaa %s %d", "bbb", 12)` 에서 bbb 가 fmt, args가 가변인자



## Extra_CFLAGS :

https://lwn.net/Articles/21835/

https://stackoverflow.com/questions/27862627/what-is-the-purpose-of-extra-cflags



## KBUILD_EXTRA_SYMBOLS





## 실습

커널 모듈을 컴파일하기 위한 커널 소스코드 :

sudo apt-get install -y build-essentail linux-headers-$(uname -r)







## Challenge #1

첫번째 연습문제는 전역변수 (jy_id)를 export했다.



challenge에서는 함수를 export하기로 한다.

my_id를 전역변수로 두고

get_my_id, set_my_id로 my_id를 setting하기.



### 문제해결방식



파일 : ch1_mod1_201512285.c, ch1_mod2_201512285.c



ch1_mod1*.c

​	static int my_id;

​	get_my_id, set_my_id를 선언, 그리고 정의

ch1_mod2*.c

​	ppt에 있는 대로 set_my_id와 get_my_id를 활용해서 파일 make후, 실행해보고, dmesg로 확인 후 제출. ( dmesg 시 커널 로깅 우선순위 설정/확인 필요)



### including functions by extern
<br/>

```c
#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

int lkmc_dep = 0;
EXPORT_SYMBOL(lkmc_dep);
static struct task_struct *kthread;

static int work_func(void *data)
{
    while (!kthread_should_stop()) {
        printk(KERN_INFO "%d\n", lkmc_dep);
        usleep_range(1000000, 1000001);
    }
    return 0;
}

static int myinit(void)
{
    kthread = kthread_create(work_func, NULL, "mykthread");
    wake_up_process(kthread);
    return 0;
}

static void myexit(void)
{
    kthread_stop(kthread);
}

module_init(myinit)
module_exit(myexit)
```

```c
#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

extern int lkmc_dep;
static struct task_struct *kthread;

static int work_func(void *data)
{
    while (!kthread_should_stop()) {
        usleep_range(1000000, 1000001);
        lkmc_dep++;
    }
    return 0;
}

static int myinit(void)
{
    kthread = kthread_create(work_func, NULL, "mykthread");
    wake_up_process(kthread);
    return 0;
}

static void myexit(void)
{
    kthread_stop(kthread);
}

module_init(myinit)
module_exit(myexit)
```

