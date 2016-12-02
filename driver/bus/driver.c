/**************************************************************


在之前 button_interrupt_waitqueue:button.c 是通过挂载linux模块方式 完成按键中断设备在linux上的注册挂载

而这个例子是通过总线设备来注册 完成按键中断设备在linux上的注册挂载


重点内容:
bus.c
include/linux/device.h
struct bus_type {			//一条总线的描述结构
	const char	*name;		//定义总线的名字
	int (*match)(struct device *dev, struct device_driver *drv);	//总线匹配函数,一般总线挂入设备/驱动信息时便自动执行的函数
	...}
extern int __must_check bus_register(struct bus_type *bus);	//分配总线设备类次设备号,注册并挂载一个总线设备到linux上,总线设备创建了一条总线
extern void bus_unregister(struct bus_type *bus); //从linux上卸载一个总线设备,总线设备删除了一条总线,释放总线设备类次设备号,

device.h
include/linux/device.h
struct device {				//一个设备信息的描述结构
	const char	*init_nam	//定义设备的名字
	struct bus_type	*bus;		//要挂入的 总线 的名字
	struct kobject kobj;		//记录其他内核信息,特别第,把一个设备信息挂入总线时,会先把 init_nam 内容拷到kobj.name,然后置空 init_nam
	...}
extern int __must_check device_register(struct device *dev);	//把一个设备信息挂入总线
extern void device_unregister(struct device *dev);	//从某总理里注销一个设备信息

driver.h
include/linux/device.h
struct device_driver {			//一个驱动信息的描述结构
	const char	*name;		//定义驱动的名字
	struct bus_type	*bus;		//要挂入的 总线 的名字
	int (*probe) (struct device *dev);  //一旦匹配策划内故宫,就执行的函数,一般在这里分配XX设备类次设备号,注册并挂载一个XX设备到linux上
	int (*remove) (struct device *dev); //匹配成功后,从总线上注销 设备或驱动 信息时,就要执行的函数,一般在这里从linux上卸载设备,释放设备号
	...}
extern int __must_check driver_register(struct device_driver *drv);	//把一个驱动信息挂入总线
extern void driver_unregister(struct device_driver *drv);	//从某总理里注销一个驱动信息



编译调试通过(注意makefile内容,生成三个内核模块)
开发板上:	insmod bus.ko	//分配总线设备类次设备号,注册并挂载一个总线设备到linux上,总线设备创建了一条key_bus_type总线
		insmod device.ko	//往key_bus_type注册 设备信息,总线马上匹配,没发现驱动信息
		insmod driver.ko	//往key_bus_type注册 驱动信息,总线又一次匹配,成功!触发在driver.ko里的peobe函数
					//在probe里工作, 分配杂项设备类次设备号,注册并挂载一个按键中断设备到linux上
		./app		//执行对应的用户程序
		按键有相应
		rmmod device.ko	//从key_bus_type卸载 设备信息,触发driver.ko里的remove函数
				//在remove里工作, 从linux卸载按键中断设备,并释放杂项设备类次设备号
		按键没有反应
		insmod device.ko	//重新往key_bus_type注册 设备信息,总线匹配,成功!触发在driver.ko里的peobe函数
		按键有相应
		rmmod driver.ko	//从key_bus_type卸载 驱动信息,卸载驱动信息前 先自动触发driver.ko里的remove函数
				//在remove里工作, 从linux卸载按键中断设备,并释放杂项设备类次设备号
		按键没有反应
		insmod driver.ko	//重新往key_bus_type注册 驱动信息,总线匹配,成功!触发在driver.ko里的peobe函数
		按键有相应
		....
**************************************************************/


//linux模块加载使用的头文件
#include <linux/module.h>
#include <linux/init.h>
//与总线设备相关头文件
#include <linux/device.h>
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

#define GPH2CON		0xe0200c40

extern struct bus_type key_bus_type;
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

int key1_probe(struct device *dev)	//主要工作是 注册挂载设备到linux上, 与button_interrupt_waitqueue:button.c:button_init 一样
{

	int ret;	
	printk("init_key1\n");
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

int key1_remove (struct device *dev)	//主要工作是从linux上 卸载设备,与button_interrupt_waitqueue:button.c:button_exit 一样
{						
	//从linux卸载一个杂项设备驱动,
	misc_deregister(&misc_button);

	//注销中断处理函数
	free_irq(IRQ_EINT(16),(void *)1);	//irgno:中断号,1:irq_desc里的设备号

    return 0;
}

struct device_driver key1_driver = {
    .name = "key1_dev",		//总线设备匹配函数mach 匹配的内容
    .bus = &key_bus_type,	
    .probe = key1_probe,	//当总线key_bus_type mach成功后 调用 函数
    .remove = key1_remove,	//当设备信息 从总线上注销时, 或者 驱动信息 从总线上注销时 都会调用的函数
};

int key1_driver_init(void)				//模块初始化
{
	int ret;
	
	ret = driver_register(&key1_driver);	//表示模块的初始化 是 往总线key_bus_type挂驱动信息,而不是 注册挂载设备到linux上
	
	return ret;
}


void key1_driver_exit(void)
{
	driver_unregister(&key1_driver);	// 从总线key_bus_type 卸载驱动信息
}


module_init(key1_driver_init);
module_exit(key1_driver_exit);
