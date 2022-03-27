Basic Kernel Functions

# Memory Allocation

```c
//#include <linux/vmalloc.h>

void *vmalloc(unsigned long size)

void *vfree(cont void *addr)
```

```c
void    *kmalloc(size_t size, int flags)


void    kfree(const void *addr)
```

### kmalloc 의 Flag

- GFP_KERNEL : 메모리가 충분하지 않을 때 blocked 되어 대기

- GFP_ATOMIC : block되면 안되는 프로세스를 위해 (interrupt handler)

- GFP_DMA : DMA를 위해...
  
  - 물리메모리가 연속적이어야 한다.
  
  - 물리메모리 주소값이 작은 경우

# Linked list





* list의 head는 [0, 1, 2, 3, 4,...... 10]일 때, 10이 tail이며, 0이 head

```c
INIT_LIST_HEAD(struct list_head *list)

list_add(struct list_head *new, struct list_head *head)
list_add_tail(struct list_head *new, struct list_head *head)

list_del(struct list_head *entry)
list_del_init(struct list_head *entry)

list_empty(struct list_head *head)
```

Traverse

```c
list_for_each(struct list_head *pos, struct list_head *head)  // pos가 ext node 를 가리키도록.


list_for_each_safe(struct list head *pos, struct list_head *n, struct list_head *head) 
//삭제시 사용

// 현재 위치 -pos-의 node를 반환.
list_entry (struct list_head *pos, [struct type], [member name])
```

```c
llist_for_each_entry (	pos,
 	head,
 	member);

```

> 1. list_head를 포함하는 사용자 정의 구조체 생성.
> 
> 2. head 역할을 해줄 구조체 생성
> 
> 3. init 함수 정의
>    
>    1. INIT_LIST_HEAD(list_head structure)
>    
>    2. kmalloc으로 생성
>    
>    3. list_add로 넣어줌
>    
>    4. 2~3 반복
>    
>    5. list_for_each로 2~3에서 넣어준 값 확인
>    
>    6. list_for_each_entry로 확인



> 1. __exit 작성.
> 
> 2. for_each_safe로 지우기
>    
>    1. list_del 사용하기









# User Memory Access

```c
unsigned long copy_from_user( void *to, const void __user *from, unsigned long n)
{
    
}

unsigned long copy_to_user(void __user *to, const void *from, unsigned long n)




```

  

```c
//KERNEL LEVEL
static ssize_t simple_basic_kernel_write(struct file *flip, const char *buf\
    , size_t len, loff *lof)
{
    copy_from_user(k_buff, buf, len);
    return (0);
}

static ssize_t simple_basic_kernel_read(struct file *flip, const char *buf, \
    , size_t len, loff_t *lof)
{
    copy_to_user(buf, k_buff, len);
    return (0);
}

```

```c
//USER LEVEL
char w_buff[100];
char r_buff[100];

strcpy(w_buff, "testSTRING");
fd = open("/dev/DEVNAME, O_RDWR);

write(fd, w_buff, 100);
read(fd, r_buff, 100);
```







# SpinLock

* include <linux/spinlock.h>



```c
#include <linux/spinlock.h>


spin_lock_init(spinlock_t *lock)
spin_lock(spinlock_t *lock)
spin_unlock(spinlock_t *unlock)
```



# Delay

```c
void udelay(unsigned long usecs)
```







# 프로그램 만들기



## 만들 프로그램 지침



       copy_to_user, copy_from_user를 활용한 프로그램을 만드려고 하는데,

device file은 user level의 여러 프로세스에서 접근하고 열 수 있기 때문에 lock이 필요하다.



```c
struct str_st *kern_buf;    //커널 안의 메모리 영역
spinlock_t my_lock;


void delay
```















## Challenge 3

* 메시지 저장 커널 모듈

* driver file로 열고, driver module의 ioctl로 제작한 function을 활용하여 kernel 안에 있는 linked list 안에 메시지로 저장하고, ioctl을 통해서 다른 command로 요청하면, linked list 안에 있는 메시지를 반환해주는, 그런 커널 모듈을 개발하는것이 목적



* headerfile이 존재함 
  
  * ch3.h

* **ch3_mod_학번.c**
  
  * 연결리스트 구조체는 통일.
  
  * msg_list_head라는 head 구조체가 존재함
  
  * ioctl : `CH3_IOCTL_WRITE, CH3_IOCTL_READ`
    
    * `CH3_IOCTL_WRITE : `  매크로로 받은 구조체를 msg_list 뒤에 추가.
    
    * `CH3_IOCTL_READ` : 매크로로 받은 msg_st를 반환하면서, 커널 영역에서는 리스트 삭제.
  
  * ARG : 
    
    * CH3_IOCTL_WRITE : 메시지가 저장되어있는 구조체의 포인터.
    
    * CH3_IOCTL_READ : 메시지 구조체를 읽어올 User Buffer의 포인터.





* 예외사항 :
  
  * `CH3_IOCTL_WRITE :`
    
    * mst_st.len이 128보다 큰 경우 -> 발생하지 않는다고 가정.
    
    * 커널 메모리 부족 -> 발생하지 않는다고 가정
  
  * `CH3_IOCTL_READ : `
    
    * arg가 가리키는 버퍼의 크기가 msg_st보다 작은 경우 ->발생하지않는다.
    
    * msg_list_head가 없는 경우 : (Read 요청이 왔는데 ) 





## Challenge 3 요구사항

개요 : kernel 안에서는 linked list로 구현한 message queue를 들고 있으면서 user가 파일을 요구할때 메시지 큐에 메시지를 집어넣을 수 있어야 함. 반대로, user가 messgae를 읽을 때, message를 반환하면서, message를 linked list에서 뺄 수 있어야한다.

1    0   0   0   0   0   0   0    2

head                                    tail 이라고 할때,  IOCTL_WRITE는 tail에 메시지를 저장하고, IOCTL_READ는 head에서 메시지를 빼는 방식.



## 구조

```shell
ch3.h
ch3_mod_201512285.c
mknod.sh
Makefile
ch3_reader_1.c
ch3_writer_2.c
ch3_reader_1.c
ch3_writer_2.c
```



**ch3_mod_201512285.c**



```c
//헤더 추가

//연결리스트 초기화
// entry head + LIST_INIT

ch3_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
    // CH3_IOCTL_READ() : ioctl_read(arg)
    // CH3_TOCTL_WRITE() : ioctl_write(arg)

ioctl_read(arg : )
{
    //linked_list entry head는 밖에 선언되어있어서 매개변수로 안들고 다녀도 됨.
    //head에 있는 것을 msg_st를 읽어서 반환.
          //반환시에 return value로 반환하는가? 아니면 인자의 주소로 반환하는 가?
    //큐에서 삭제.
}

ioctl_write(arg : 메시지 전달.)
```



헤더 추가

연결리스트 초기화

 entry head + LIST_INIT

ch3_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
CH3_IOCTL_READ() : ioctl_read(arg)
CH3_TOCTL_WRITE() : ioctl_write(arg)

`ioctl_read(arg : )`
{
    linked_list entry head는 밖에 선언되어있어서 매개변수로 안들고 다녀도 됨.

읽는 중에는 spin_lock 걸어두기
    head에 있는 것을 msg_st를 읽어서 반환.
    반환시에 return value로 반환하는가? 아니면 인자의 주소로 반환하는 가?

   큐에서 삭제할 때는 spin_lock 걸어두기

   msg_list는 삭제해주어야 하는가?

   큐에서 삭제.
}





`ioctl_write(arg : msg_st.)`

{

    new linked_list 구조체 만들기.

구조체에 msg_st assign하기.

    linked_list_add_tail()로 전달.    

}





* fops 등록

* cdev, dev_num 선언

* cdev, dev_num 선언

__ init에서 연결리스트 등록 필수.

__ init에서 cdev, dev_num 초기화 필수.




