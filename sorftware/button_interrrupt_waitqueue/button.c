/*************************************************************



linux中断:
(include/linux/interrupt.h)
static inline int __must_check
request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
	    const char *name, void *dev)
	__must_check:	(include/linux/compiler-gcc.h)
			define __must_check		__attribute__((warn_unused_result))
	irg:	中断号,从对应开发板的irqs.h里分析出来
	handler: 中段处理函数的指针
	flags:	IRQF_DISABLED (SA_INTERRUPT)	(这些宏定义在 include/linux/interrupt.h)
			设置了,表示是快速中断,否则是慢速中断,快速中断指,中断进行时,下一个中断发生时会被忽略,
		IRQF_SHARED (SA_SHIRQ)
			表示中断号是共享中断的中断号,不选表示非共享中断
		....
	name:	中断的名字,不是中断所在的驱动设备的名字!!
	void *:	特别意义的参数,后面一起讨论	
typedef irqreturn_t (*irq_handler_t)(int, void *)
	int:	放置中断号
	void *:	特别意义的参数,后面一起讨论	
extern void free_irq(unsigned int, void *)
	int:	放置中断号
	void *:	与上述两者一样的参数,现在统一讨论:
		一般来说,一个驱动可能需要多个中断,而且,为了容易管理,用户会自定义一个统一中断描述中断结构来描述各个中断例如:
		struct button_desc {			//这是一个按键中断的统一描述结构,
			int gpio;			//包含中断处理函数,或者中断初始化,申请中断,都可能使用的各种资源
			int number;
			char *name;	
			struct timer_list timer;
		};
		static struct button_desc buttons[] = {		//这里定义了4个 按键中断
			{ S5PV210_GPH2(0), 0, "KEY0" },		//每个中断都填写相关的资源内容
			{ S5PV210_GPH2(1), 1, "KEY1" },
			{ S5PV210_GPH2(2), 2, "KEY2" },
			{ S5PV210_GPH2(3), 3, "KEY3" },
		}:
		例如:第0个按键中断 buttons[0],使用 S5PV210_GPH2(0)这gpio针触发中断,中断名字是"KEY0"等等..
		request_irq最后参数,我们可以使用这个按键中断的描述结构的始地址 &buttons[0],
		来代表这个中断事件的id号,挂载到指定中断号的irq_desc描述结构里,从而区别同一个中断号里的各个中断事件
		在 中断处理函数irq_handler里,这个中断事件的id号会作为参数传递给处理函数,而id号本身是一个地址
		而中断函数,强制类型转换这id为一个中断描述结构变量的地址,便得到这个中断处理函数可能需要用到的资源
		所以, &buttons[0] 便一号多用了.
		最后,free_irq,使用这个 &buttons[0] 也是当做是这个按键中断对应的中断事件的id号,从而把这个中断事件从中断号上注销掉


调试成功insmod,但是按键中断失败.后来发现是 request_irq 里的irq号设置错误.不是IRQ_EINT16_31 而是IRQ_EINT(16)

从s5pv210的逻辑按键中断看,由于是基于vic0技术的中断,
所以初始化函数,需要汇编代码开启中断开关,中断函数要手动保护环境,手动回复环境.
而且中断函数始地址要手动放到指定寄存器里

在内核并不必要关心,是不是vic技术,还是中断向量表统一入口技术,只要:
	request_irq 申请中断,中断号从对应开发板的irqs.h里分析出来
	定义中断函数,
		中断函数不必要考虑 手动保护环境,手动回复环境的操作 的问题
		而且不必要考虑 中断函数始地址要手动放到指定寄存器里 的问题
	触发中断的硬件初始化,例如按钮,与裸机对应的初始化相同.

//////////////////////////////等待队列

同时测试等待队列:
	//初始化,
	wait_queue_head_t wq1;		//定义一条等待队列
	init_waitqueue_head(&wq1);	//由于中断会使用,先于request_irq前执行
	//使进程加入等待队列
	wait_event_killable(wq1, c);	//推荐使用wait_event_killable,非wait_event,因为TASK_KILLABLE比TASK_UNINTERRUPTIBLE更好
	//唤醒进程
	wake_up(&wq1);		//唤醒wq1

编译测试:
	用户进程app只使用了read,然后来到button_read()时便阻塞了,进程访问到button_read时,已经是运行在内核态了,
	即进程从用户态运行到内核态,但是依然是同一个进程.
	中断触发后,由另一个进程执行处理函数,中断进程唤醒了休眠的进程.

	最后的一个疑问,两个不一样的进程 执行 同一用户程序,而程序的全局变量c,对于两个不一样的进程来说是各自独立的,
	即进程A访问的c,与进程访问的c是相对独立的.	因为c会从程序里取出,放到各自进程的栈区
	
	然而,这里,内核态用户进程,与 中断进程访问的 c变量 却是同一个变量,所以要注意!!!!!
	

**************************************************************/


//linux模块加载使用的头文件
#include <linux/module.h>
#include <linux/init.h>
//中断相关头文件
#include <linux/miscdevice.h>
#include <linux/kernel.h>	//printk
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
//等待队列相关头文件
#include <linux/wait.h>	
#include <linux/sched.h>		//关于TASK_UNINTERRUPTIBLE 一类宏

#define GPH2CON		0xe0200c40
//#define GPH2_0_int	(0xf<<(0*4))
//#define GPH2_0_msk	(0xf<<(0*4))

wait_queue_head_t wq1;		//定义一条等待队列

int c = 0;			//测试等待队列的变量

//中断处理函数
irqreturn_t button_inter (int irq, void *dev_id)
{
	//检测是否自己的中断(共享中断)
	//清除发生的中断状态,使之可以发生下一次中断
	//处理内容
	printk(KERN_EMERG "push !!\n");
	
	c = 999;

	wake_up(&wq1);		//唤醒wq1
	
	return IRQ_HANDLED;
}
	
//按钮硬件初始化
void hw_init(void)
{
	unsigned int *gpio_config;
	unsigned int data;

	//取出寄存器地址
	gpio_config = ioremap(GPH2CON,4);

	//组织数据
	data = readw(gpio_config);		
	data &= ~0b11111111;
	data |= 0b11111111;
	
	//填入数据
	writew(data,gpio_config);

}

int button_open (struct inode *n, struct file *fd)
{
	return 0;
}

ssize_t button_read (struct file *filp, char __user *buff, size_t count, loff_t *pos)
{
	int ret_r;
	wait_event_killable(wq1, c);
	printk(KERN_EMERG "button_read:\n");
	ret_r = copy_to_user(buff,&c,4);
	c = 0;
	
	return ret_r;
}


int button_close (struct inode *n, struct file *fd)
{
	return 0;
}

struct file_operations button_ops ={
	.open = button_open,
	.read = button_read,
	.release = button_close,
};

struct miscdevice misc_button ={
	.minor = 200,
	.name = "button",
	.fops = &button_ops,
};

static int button_init(void)
{
	int ret;	

	printk(KERN_EMERG "irq_init_start\n");

	//往linux挂载一个杂项设备驱动,
	//其实就是关联一个次设备号为200的设备驱动到主设备为10的字符设备集里,
	//自动生成对应逻辑设备文件并映射
	misc_register(&misc_button);

	//初始化等待队列wq1
	init_waitqueue_head(&wq1);

	//注册中断处理程序
	//IRQ_EINT(16):中断号, button_inter:中断处理函数, flags:IRQF_TRIGGER_FALLING表示下降沿触发中断,
	//button1:中断的名称, 1:中断号irq_desc里挂载关于这个中断的中断事件id号,从而区别使用同一个中断号里的各个中断事件(主要为共享中断服务)
	ret = request_irq(IRQ_EINT(16),button_inter,IRQF_TRIGGER_FALLING,"button1",(void *)1);	

	//按键硬件初始化
	hw_init();

	printk(KERN_EMERG "irq_init_complete\n");
	return 0;
}

static int button_exit(void)
{

	//从linux卸载一个杂项设备驱动,
	misc_deregister(&misc_button);

	//注销中断处理函数
	free_irq(IRQ_EINT(16),(void *)1);	//irgno:中断号,1:irq_desc里的设备号

	return 0;
}

MODULE_LICENSE("GPL");

module_init(button_init);
module_exit(button_exit);
