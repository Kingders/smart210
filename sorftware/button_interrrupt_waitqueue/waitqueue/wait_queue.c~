/*************************************************************

模型1:init->funca->wait_event->这时modinit还没有完全实现便永久阻塞在TASK_UNINTERRUPTIBLE,所以建议使用wait_event_killable,
起码阻塞在TASK_KILLABLE 可以使用ctrl+c结束

使用内核的初始化模块来进行wait_queue并不明智,因为,模块初始化函数一般不能fork()出多进程
所以本例子测试例子是错误的.

然贺奇怪的是,如果linux模块是驱动程序,
某用户进程访问通过内核调用转到一个内核进程1访问模块内容A,由于阻塞而休眠
而定时器事件触发了另一个内核进程2,修改了内容A,并唤醒内核进程1
内核进程1,居然可以访问到 内核进程2修写的内容A,
也就是说 内容A,对于两个不同的进程,是共享内容,但是,哪里标示了这是共享内容?,两者的内容A应该互相独立才对.

**************************************************************/


//linux模块加载使用的头文件
#include <linux/module.h>
#include <linux/init.h>
//其他需要的头文件
#include <linux/wait.h>	
#include <linux/sched.h>		//关于TASK_UNINTERRUPTIBLE 一类宏
//#include <linux/vmalloc.h>		//kmalloc, 用vfree()
//#include <linux/slab.h>		//kmalloc,在cache分配空间, 用kfree()

wait_queue_head_t wq1;		//定义一条等待队列

int c = 0;	


void funca (void)
{
	printk(KERN_EMERG "fa 1\n");
	wait_event_killable(wq1, c);
	printk(KERN_EMERG "fa c:\n",c);
	exit(1);
}

void funcb (void)
{
	printk(KERN_EMERG "fb 1\n");
	c += 2;
	wake_up(&wq1);
	printk(KERN_EMERG "fb c:\n",c);
	exit(1);
}

void funcc (void)
{
	printk(KERN_EMERG "fc 1\n");
	wait_event_killable(wq1, c);
	printk(KERN_EMERG "fc c:\n",c);
	exit(1);
}


int main(void)
{
	printk(KERN_EMERG "start\n");

	//初始化等待队列wq1
	init_waitqueue_head(&wq1);

	//定义并初始化等待队列wq2
	//DECLARE_WAIT_QUEUE_HEAD(wq2); 

	if (!fork())	//子进程1
		funca();
	if (!fork())	//子进程2
		funcc();
	sleep(4);
	if (!fork())	//子进程3
		funcb();

	return 0;
}

