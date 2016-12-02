/**************************************************************

平台总线设备重要内容:
key_dev.c:
include/linux/platform_device.h
struct platform_device {		//定义一个迎合平台总线的 设备信息
	const char	* name;		//设备的名字 (设备的名字与驱动的名字都用于platform总线匹配函数mach,相同则表示匹配)
	int		id;		//设备ID号:但不用于匹配,未知用途
	struct device	dev;		//通用设备信息结构,迎合平台总线的设备信息 是 通用设备信息的分支,所以使用通用信息结构保存其他一般信息
	u32		num_resources;	//资源数目,资源是平台总线设备引入的一个特别功能.
	struct resource	* resource;	//资源的地址
	...}
include/linux/ioport.h
struct resource {			//资源的描述结构
	resource_size_t start;		//资源的开始位置
	resource_size_t end;		//资源的结束位置
	const char *name;		//资源的名称
	unsigned long flags;		//标记资源的类型
	struct resource *parent, *sibling, *child;	//链接到其他资源描述结构的位置
};
extern int platform_device_register(struct platform_device *);		//把设备信息挂入平台总线
extern void platform_device_unregister(struct platform_device *);	//从平台总线里注销设备信息

key_drv.c
include/linux/platform_device.h	
struct platform_driver {				//定义一个迎合平台总线的 驱动信息			
	int (*probe)(struct platform_device *);		//匹配成功后执行的函数,一般用于 注册挂载设备到linux上
	int (*remove)(struct platform_device *);	//设备/驱动其中一方信息从平台总线注销时,执行的函数,一般用于从linux上卸载设备
	void (*shutdown)(struct platform_device *);
	int (*suspend)(struct platform_device *, pm_message_t state);
	int (*resume)(struct platform_device *);
	struct device_driver driver;	//通用驱动信息结构,迎合平台总线的驱动信息 是 通用驱动信息的分支,所以使用通用驱动结构保存其他一般信息
	const struct platform_device_id *id_table;
};
extern int platform_driver_register(struct platform_driver *);		//把驱动信息挂入平台总线
extern void platform_driver_unregister(struct platform_driver *);	//从平台总线里注销驱动信息
//获取资源函数
//struct platform_device *	//平台设备
//unsigned int			//flags: 标记资源的类型
//unsigned int			//表示取其中 第 X 个符合资源的类型 的资源
extern struct resource *platform_get_resource(struct platform_device *, unsigned int, unsigned int);

资源的使用例子:
static struct resource key_resource[] = {	//初始化了两个资源
	[0] = {
		.start = GPH2CON,
		.end   = GPH2CON + 3,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_EINT(16),
		.end   = IRQ_EINT(16),
		.flags = IORESOURCE_IRQ,
	},
	[2] = {
		.start = GPH1CON,
		.end   = GPH1CON + 3,
		.flags = IORESOURCE_MEM,
	},
};
struct platform_device key_device = {
	.name		  = "key1",
	.id		  = 0,
	.num_resources	  = ARRAY_SIZE(key_resource),	//算出数列 key_resource[] 有两个成员
	.resource	  = key_resource,		//填写资源的地址
};

platform_get_resource(pdev, IORESOURCE_MEM, 0);	注意最后一个参数用法, 表示取 key_resource[0] 这个资源
platform_get_resource(pdev, IORESOURCE_MEM, 1);	注意最后一个参数用法, 表示取 key_resource[2] 这个资源


一般资源都是用于规范初始化和控制硬件

编译调试:与bus例子一样成功



**************************************************************/

//linux模块加载使用的头文件
#include <linux/module.h>
#include <linux/init.h>
//平台设备头文件
#include <linux/platform_device.h>
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


MODULE_LICENSE("GPL");

wait_queue_head_t wq1;		//定义一条等待队列
int c = 0;			//测试等待队列的变量

struct resource *res_mem;
struct resource *res_irq;
unsigned int *key_base;

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
	unsigned int data;
	//组织数据
	data = readw(key_base);
	data &= ~0b11111111;
	data |= 0b11111111;
	
	//填入数据
	writew(data,key_base);

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


int key_probe(struct platform_device *pdev)
{
	int ret, size;	

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
	res_irq =  platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	ret = request_irq(res_irq->start,button_inter,IRQF_TRIGGER_FALLING,"button1",(void *)1);	

	//按键硬件初始化
	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	size = (res_mem->end - res_mem->start) + 1;
	key_base = ioremap(res_mem->start, size);
	hw_init();

	printk(KERN_EMERG "irq_init_complete\n");

    return 0;
}

int key_remove(struct platform_device *dev)
{
	free_irq(res_irq->start, (void *)1);

	iounmap(key_base);
	misc_deregister(&misc_button);
	return 0;
}

static struct platform_driver key_driver = {
	.probe		= key_probe,
	.remove		= key_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "key1",
	},
};

static int button_init(void)
{
    return platform_driver_register(&key_driver);
}


static void button_exit(void)
{	   
    platform_driver_unregister(&key_driver);
}


module_init(button_init);
module_exit(button_exit);


