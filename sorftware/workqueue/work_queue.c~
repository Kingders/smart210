/*************************************************************

工作队列描述结构:
(kernel/woekqueue.c)
struct workqueue_struct {	//工作队列描述结构
	...
(include/linux/workqueue.h)
struct work_struct (		//工作描述结构
	...
	work_func_t func;	//工作要执行的函数

使用:
创建工作队列: create_workqueue
(include/linux/workqueue.h)
#define create_workqueue(name)						\
创建工作: INIT_WORK
(include/linux/workqueue.h)
#define INIT_WORK(_work, _func)						\
提交工作: queue_work
(include/linux/workqueue.h)
static inline bool queue_work(struct workqueue_struct *wq,
			      struct work_struct *work)


**************************************************************/


//linux模块加载使用的头文件
#include <linux/module.h>
#include <linux/init.h>
//其他需要的头文件
#include <linux/workqueue.h>
//#include <linux/vmalloc.h>	//kmalloc, 用vfree()
#include <linux/slab.h>		//kmalloc,在cache分配空间, 用kfree()
//#include <linux/kernel.h>	//printk

struct workqueue_struct *q;	//工作队列指针

struct work_struct *w1, *w2, *w3;	//工作指针	


void work1_func (struct work_struct *work)
{
	printk(KERN_EMERG "this is work 1\n");
}

void work2_func (struct work_struct *work)
{
	printk(KERN_EMERG "this is work 2\n");
	queue_work(q,w1);	
}

void work3_func (struct work_struct *work)
{
	printk(KERN_EMERG "this is work 3\n");
}


static int workqueue_init(void)
{
	printk(KERN_EMERG "start\n");
	//创建名字为sample的工作队列
	q = create_workqueue("sample");		

	//创建工作w1
	w1 = kmalloc(sizeof(struct work_struct),GFP_KERNEL);
	INIT_WORK(w1, work1_func);

#if 1
	//创建工作w2
	w2 = kmalloc(sizeof(struct work_struct),GFP_KERNEL);
	INIT_WORK(w2, work2_func);

	//创建工作w2
	w3 = kmalloc(sizeof(struct work_struct),GFP_KERNEL);
	INIT_WORK(w3, work3_func);
#endif

	//提交工作
	queue_work(q,w1);		//除了提交外 ,第一次提交会自动为运行队列q的内容而建立一个新的并行的线程
	queue_work(q,w3);	     
	queue_work(q,w2);
	//INIT_WORK(w1, work1_func);	//注意,不能这样会造成死机的,原因w1挂载了工作队列还没有执行,
	//queue_work(q,w1);		//由于可能w1挂载了工作队列而还没有执行,所以这种重复递交会忽略
	schedule_work(w3);	//提交 w3 到系统默认已建立的工作队列 keventd_wq
				//注意:这时,有w3在工作队列q里未执行,所以w3不会加到keventd_wq里去,若没有 queue_work(q,w3); 则成功啊挂载
				//也就是说,一个工作挂载到一个队列里,只要它还没有结束,就不能把同一个工作重新挂到任意工作队列里

	return 0;
}

static int workqueue_exit(void)
{
	kfree(w1);
	kfree(w2);
	kfree(w3);

	return 0;
}

MODULE_LICENSE("GPL");

module_init(workqueue_init);
module_exit(workqueue_exit);
