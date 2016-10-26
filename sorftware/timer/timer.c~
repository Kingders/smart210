/*************************************************************

定时器事件:当时钟到达未来某个时间点时,触发并执行一项任务
这个未来的时间点有点特别!!
自内核运行起来后,计算机运行在主频上,例如 2.5GHZ, 而 1秒/2.5GHZ = 一个时钟周期经历的时间,就是1hz经历的时间
而 0.01GHZ 经历的时间,我们称为 一个单位时间
内核运行起来后,内核统计着计算机到目前为止所有经历的单位时间总数,通过jiffies宏显示.
而这里未来的时间点表示:到达未来的某个 单位时间后,并执行一项任务,
未来的某个单位时间 - 目前为止所有经历的单位时间总数 = 一段定时器事件触发前等待的时间

另外宏 HZ 也是以 单位时间 为单位 2.5GHZ主频 对应宏HZ为250

(include/linux/timer.h)
struct timer_list {						//定时器事件描述结构
	...
	unsigned long		expires;			//定时器未来时间点,单位见上
								//
	void			(*function)(unsigned long);	//执行函数


初始化一个定时器事件.
(include/linux/timer.h)
#define init_timer(timer)						\

add_timer 注册定时器事件到linux内核
(kernel/time/timer.c)
void add_timer(struct timer_list *timer)


mod_timer 设置事件未来时间点并执行一次定时器顺数事件 
再次 mod_timer 再次设置事件未来时间点并执行新一次定时器顺数事件
(include/linux/timer.h)
extern int mod_timer(struct timer_list *timer, unsigned long expires);

(include/linux/raid/pq.h)		//包含timer.h便已经包括了,
# define jiffies	raid6_jiffies()


**************************************************************/


//linux模块加载使用的头文件
#include <linux/module.h>
#include <linux/init.h>
//其他需要的头文件
#include <linux/timer.h>
//#include <linux/vmalloc.h>		//kmalloc, 用vfree()
//#include <linux/slab.h>		//kmalloc,在cache分配空间, 用kfree()

struct timer_list t1;		//定时器事件 1 描述结构
	


void funca (unsigned long a)
{
	printk(KERN_EMERG "action a\n");
}

void funcb (unsigned long b)
{
	printk(KERN_EMERG "action b\n");
	//mod_timer(&t1, jiffies + HZ);	//实验证明,可以这样重新激活一次定时器事件,
					//但是不建议简单这样使用,因为,内核卸载前不能存在未完成的定时器事件,不然会死机
}

void funcc (unsigned long c)
{
	printk(KERN_EMERG "action c\n");
}


static int timer_init(void)
{
	printk(KERN_EMERG "start,HZ:%d jiffies:%d\n",HZ,jiffies);

	//初始化定时器事件
	init_timer(&t1);	//初始化定时器
	t1.function = funcb;	//初始化事件

	//注册定时器事件到linux内核
	add_timer(&t1);

	//设置一未来时间点,执行一次定时器事件.
	mod_timer(&t1, jiffies + HZ/2);		//jiffies目前为止已经经历了的 单位时间, 
						//HZ为250,单位是 单位时间,HZ/2表示0.5秒
						//当前cpu频率 = 1秒 / 一个单位时间

	return 0;
}

static int timer_exit(void)
{


	return 0;
}

MODULE_LICENSE("GPL");

module_init(timer_init);
module_exit(timer_exit);
