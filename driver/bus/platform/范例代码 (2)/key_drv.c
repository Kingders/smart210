#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");

struct work_struct *work;

struct timer_list buttons_timer;

unsigned int key_num = 0;

wait_queue_head_t  key_q;

struct resource *res;
struct resource *res_irq;
unsigned int *key_base;

void work_func(struct work_struct *work)
{
    mod_timer(&buttons_timer, jiffies + (HZ /10)); 	
}

void buttons_timer_function(unsigned long data)  
{
    unsigned int key_val;
    
    key_val = readw(key_base+1)&0x1; 
    if (key_val == 0)
       key_num = 4;
    
    key_val = readw(key_base+1)&0x4;
    if (key_val == 0)
        key_num = 3;
    
    wake_up(&key_q); 
} 


irqreturn_t key_int(int irq, void *dev_id)
{
    //1. 检测是否发生了按键中断
    
    //2. 清除已经发生的按键中断
    
    //3. 提交下半部
    schedule_work(work);
  
    //return 0;
    return IRQ_HANDLED;
    	
}

void key_hw_init()
{
    unsigned short data;
    	
    data = readw(key_base);
    data &= ~0b110011;
    data |= 0b100010;

    writew(data,key_base);
}


int key_open(struct inode *node,struct file *filp)
{
    return 0;	
}

ssize_t key_read(struct file *filp, char __user *buf, size_t size, loff_t *pos)
{ 
    wait_event(key_q,key_num);
    	
    copy_to_user(buf, &key_num, 4);
    
    key_num = 0;
  
    return 4;
}

struct file_operations key_fops = 
{
    .open = key_open,
    .read = key_read,	
};

struct miscdevice key_miscdev = {
    .minor = 200,
    .name = "key",
    .fops = &key_fops,	
};

int key_probe(struct platform_device *pdev)
{
    int ret,size;
    
    
    ret = misc_register(&key_miscdev);
    
    if (ret !=0)
        printk("register fail!\n");
    
    //注册中断处理程序
    
    res_irq =  platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	
    request_irq(res_irq->start,key_int,IRQF_TRIGGER_FALLING,"key",(void *)4);
    request_irq(res_irq->end,key_int,IRQF_TRIGGER_FALLING,"key",(void *)3);
    
    //按键初始化
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    size = (res->end - res->start) + 1;
    key_base = ioremap(res->start, size);
    
    key_hw_init();
    
    //. 创建工作
    work = kmalloc(sizeof(struct work_struct),GFP_KERNEL);
    INIT_WORK(work, work_func);
    
    /* 初始化定时器 */  
    init_timer(&buttons_timer);   
    buttons_timer.function  = buttons_timer_function;  
    
    /* 向内核注册一个定时器 */  
    add_timer(&buttons_timer);  
    
    /*初始化等待队列*/
    init_waitqueue_head(&key_q);
    
    return 0;
}

int key_remove(struct platform_device *dev)
{
	free_irq(res_irq->start, (void *)4);
	free_irq(res_irq->end, (void *)3);

	iounmap(key_base);
	misc_deregister(&key_miscdev);
	return 0;
}

static struct platform_driver key_driver = {
	.probe		= key_probe,
	.remove		= key_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "my-key",
	},
};

static int button_init()
{
    return platform_driver_register(&key_driver);
}


static void button_exit()
{	   
    platform_driver_unregister(&key_driver);
}


module_init(button_init);
module_exit(button_exit);


