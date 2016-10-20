/*************************************************************

(block/genhd.c)
int register_blkdev(unsigned int major, const char *name)	//major为0时表示自动分配主设备号,当自动分配设备号时则返回主设备号
								//注册块设备,其实就是注册块设备号

(block/blk-core.c)
struct request_queue *blk_init_queue(request_fn_proc *rfn, spinlock_t *lock)	//初始化块设备I/O请求队列
										//参数rfn:请求队列处理函数,lock:自旋锁
(include/linux/blkdev.h)
typedef void (request_fn_proc) (struct request_queue *q);	//rfn请求队列处理函数原型

(include/linux/blkdev.h)
extern void blk_queue_logical_block_size(struct request_queue *, unsigned short);  	//设置扇区尺寸
											//参数,等待队列,扇区的大小

(include/linux/genhd.h)
extern struct gendisk *alloc_disk(int minors);		//分配通用块设备描述结构
							//参数,表示这个主设备类型,最多同时允许多少个同样的硬件设备(次设备)工作
							//表示有多少个次设备号,表示同时接入多少个实际设备工作

(include/linux/genhd.h)
static inline void set_capacity(struct gendisk *disk, sector_t size)	//设置扇区数目

(block/blk-core.c)
struct request *blk_fetch_request(struct request_queue *q)	//从请求队列里取出请求

(include/linux/blkdev.h)
extern bool __blk_end_request_cur(struct request *rq, int error);	//检查当前请求是不是最后一个请求,若是返回 1 否返回0

(include/linux/blkdev.h)
static inline sector_t blk_rq_pos(const struct request *rq)		//获取请求的起始扇区,

(include/linux/blkdev.h)
static inline unsigned int blk_rq_cur_sectors(const struct request *rq)	//获取请求需要访问的扇区数目

(include/linux/blkdev.h)
#define rq_data_dir(rq)		(((rq)->cmd_flags & 1) != 0)		//获取请求方向



调试:
	1,dmesg :[  181.752624] sampleblkdev: module verification failed: signature and/or required key missing - tainting kernel
	  makefile 顶部:添加 CONFIG_MODULE_SIG=n 仍然没有用!!
	2,makefile 里对应的 KDIR 是:/lib/modules/$(shell uname -r)/build 
		   实际上就是:/lib/modules/4.2.0-42-generic/build
	3,后面一直编译出错,通过dmesg看到dump-stack发现add_disk(),发现出问题了,后来调试,关键注意配合:
		major = register_blkdev(0,"sampleblock");  
		bdev->gd->major = major;
	或是:	major = register_blkdev(72,"sampleblock"); 
		bdev->gd->major = 72;
	  配合不对,add_disk()会自动创建 逻辑设备文件并映射,对应地,del_gendisk()则自动删除建立的逻辑设备文件
	4,然后,继续出错,是因为mencpy里的 req->buffer的缘故,
		linux4.2.0 的blkdev.h的struct request 不再含有 char *buffer成员,所以不能 req->buffer这么用
		可代替地使用 void *bio_data(req->bio);
	5,此外,如果使用fdisk 做分区等行为,需要建立 getgeo 操作函数, 参考block_device3:ramblock例子
	6,最后关于 主设备,次设备,磁盘,还有磁盘分区的概念.
		1,register_blkdev() 获得主设备号,例如: 得 54
		2,每一个实体磁盘,都有一个gendisk描述结构,
		  例如,有三个同样的磁盘,所以分别有三个gendisk(gd),每个gd->major 都要等于 54 ,
		       则有三次	snprintf (..."sbull%c",...); 和三次 add_disk(gd),最终有三个逻辑设备 /dev/rama,/dev/ramb,/dev/ramc
		3,每个gendisk 都有alloc_disk(4),表示每个磁盘都 可以最多划分 3 个分区,那么次设备号的分配就得很小心以防冲突:
		  rama: gd->first_minor =0,
		  ramb: gd->first_minor =4,
		  ramc: gd->first_minor =8,
		4,简明关系表
							  主设备号 54
                        _________________________________________________________________________________
			|					|					|
		磁盘 rama 对应次设备号 0		磁盘 ramb 对应次设备号 4		磁盘 ramc 对应次设备号 8
                _________________________________________________________________________________________________
		|	|	|			|	|	|			|	|	|		
分区:	      rama0   rama1   rama2                   ramb0   ramb1   ramb2                   ramc0   ramc1   ramc2
对应次设备号:	1	2	3			5	6	7			9	10	11
	7,成功insmod sampleblkdev.ko 的现象: (root状态下才能加载)
		dmesg:除了可能的module verification failed,没有其他错误信息,特别是没有cpu堆栈错误信息dump_stack
		      (demesg也可以用于检查printk输出)
		lsmod:顶部看到 sampleblkdev 模块,并且使用者为0,(之前memcpy错误时,使用者为2,rmmod模块时会出错)
		cat /proc/devices: 出现 [主设备号] sampleblock
		ls /dev: 出现samp_blk 逻辑设备文件
	  成功rmmod sampleblkdev.ko
		lsmod cat ls 里出现的 都消失,dmesg也没有其他错误信息
	8,最后使用kmalloc 或是 kzalloc分配区别:
		kmalloc 使用vfree释放
		kzalloc 使用kfree释放	
	9,还有使用测试: (都要root权限) 
			mkfs.ext2 /dev/samp_blk		//格式化成ext2
			mount /dev/samp_blk /mnt/sampblk0	//映射到挂载目录
			然后查看属性就可以看到容量1mb,也可以cp文件等操作了
			umounr /mnt/sampblk0	//解除映射
			然后查看属性就可以看到容量为pc硬盘的剩余容量,再次mount后,进去可发现上次写入的内容
			mkdosfs /dev/samp_blk	//格式化成fat 但是要求驱动含有 getgeo 操作函数
	10,最后关于 自旋锁 和 timers 计时器在快设备的作用,还没有探究!!!
	    (未完待续)
	
编译信息:




**************************************************************/


//linux模块加载使用的头文件
#include <linux/module.h>
#include <linux/init.h>
//块设备驱动需要的头文件
#include <linux/blkdev.h>
#include <linux/bio.h>
#include <linux/vmalloc.h>
#include <linux/genhd.h>
//c库等处理函数头文件
//#include <stdio.h>	//内核模块不应该有的内容
#include <linux/errno.h>

//临时添加
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h>   /* kmalloc() */
#include <linux/fs.h>   /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/timer.h>
#include <linux/types.h> /* size_t */
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/hdreg.h> /* HDIO_GETGEO */
#include <linux/kdev_t.h>
#include <linux/buffer_head.h> /* invalidate_bdev */
#include <linux/bio.h>


#include "linux/interrupt.h"
#include "linux/mm.h"
#include "linux/ioport.h"
#include "linux/wait.h"
#include "linux/blkpg.h"
#include "linux/delay.h"
#include "linux/io.h"
 
//#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/dma.h>



#define RAMBLOCK_SIZE (512*1048)

static DEFINE_SPINLOCK(ramblock_lock);

static int major = 0;	//主设备号


static int sect_size = 512; //扇区大小等于512字节
static sector_t numsect = 2048;	//每个次设备的扇区数	共 1mb


#if 1
//自己初始化一个块设备描述结构,本来设备描述结构应该使用统一通用的,像字符设备一样
//但是硬件快设备结构不一样,导致难以形成通用统一的块设备描述结构,所以针对不同硬件,使用自定义的不同描述结构
//而这里是使用内存模拟一个银盘设备
struct blk_dev{			
	int size;	//内存(银盘设备)大小
	u8 *data;	//内存(银盘设备)数据开始位置
	struct request_queue *queue;	//块设备I/O请求队列,每个块设备描述结构都有的通用项
	struct gendisk *gd;		//gendisk,每类块设备描述结构都有的通用项,每类快设备(每个主设备号)都有的一个 通用块设备描述结构
};
struct blk_dev *bdev;	//这里建立的是指针,所以得用kmalloc分配一个空间让其指向,
			//之所以用建立指针,而不用建立变量的方式,是因为,变量有可能放入栈空间,造成栈空间浪费,
			//所以这样子使用是一种好习惯.
#endif

#if 0
static unsigned char *ramblock_buf;	//内存(银盘设备)数据开始位置
struct request_queue *queue;	//块设备I/O请求队列,每个块设备描述结构都有的通用项
struct gendisk *gd;		//gendisk,每类块设备描述结构都有的通用项,每类快设备(每个主设备号)都有的一个 通用块设备描述结构
#endif

static struct block_device_operations bdevops ={	//这里的操作函数集暂时并不需要什么操作
	.owner = THIS_MODULE,
};

#if 0
void process_request(struct blk_dev *dev,sector_t beg,unsigned int num,char *buffer,int write)
{
	unsigned long offset = beg * sect_size;		//请求操作的起始位置地址
	unsigned long nbytes = num * sect_size;	//请求操作要访问的总字节数

	//以下是(内存操作)来,模拟硬件操作

	if (rq_data_dir(req))		//如果请求是写操作
		memcpy(bdev->data+offset,buffer,nbytes);	//把buffer里的数据拷到磁盘
	else
		memcpy(buffer,bdev->data+offset,nbytes);	//把磁盘里的数据拷到buffer


}
#endif

void blk_request_process (struct request_queue *q)	//rfn请求队列处理函数原型
{
	struct request *req;	
	void *buffer;
	
	req = blk_fetch_request(q);	//从请求队列里取出一个请求
		
	while(req != NULL)		//当请求非空
	{
		//自定义 具体处理请求函数,参数有:
		//请求要操作扇区的始位置,要操作扇区的数量,数据缓存区,还有请求操作方向(读还是写)
		//process_request(bdev,blk_rq_pos(req),blk_rq_cur_sectors(req),req->buffer,rq_data_dir(req));
										//请求要操作扇区的始位置,要操作扇区的数量,
										//数据缓存区,还有请求操作方向(读还是写)
		unsigned long offset = blk_rq_pos(req) * sect_size;		//请求操作的起始位置地址
		unsigned long nbytes = blk_rq_cur_sectors(req) * sect_size;	//请求操作要访问的总字节数

		buffer = bio_data(req->bio);

		//以下是(内存操作)来,模拟硬件操作
#if 1	//memcpy 出问题
		if (rq_data_dir(req))		//如果请求是写操作
			memcpy(bdev->data+offset,buffer,nbytes);	//把buffer里的数据拷到磁盘
		else
			memcpy(buffer,bdev->data+offset,nbytes);	//把磁盘里的数据拷到buffer
#endif
	
		if( !__blk_end_request_cur(req,0))
			req = blk_fetch_request(q);	//从请求队列里取出下一个请求
		
	}	
}


static int blkdev_init(void)
{
	printk(KERN_EMERG "sampleblock_star\n");


#if 1
	major = register_blkdev(0,"sampleblock");	//blk设备驱动的注册函数打包了更多功能,对比起字符设备初始化
						//注册函数主要还是向linux申请块设备主设备号,这样就可以cat /proc/devices 查看到相关信息
						//和字符设备一样,其实注册了就可以创建逻辑设备文件和访问了,
						//但是没有映射到具体设备结构,所以并没有什么用.

#if 0
	if( major < 0 );			//出错后处理
	{
		//perror("register");	//内核模块不应该有的内容
		printk("register fair\n");
		return -EBUSY;
	}
#endif
	//对快设备驱动进行初始化
	bdev = kmalloc(sizeof(struct blk_dev),GFP_KERNEL);	//内核态空间分配一段内存空间保存一个 struct blk_dev 变量	
printk(KERN_EMERG "setup_1\n");
	//分配一段用户空间的内存来,模拟一个硬盘
	bdev->size = numsect * sect_size;
	//bdev->data = vmalloc(bdev->size);
	bdev->data = kzalloc(bdev->size,GFP_KERNEL);
printk(KERN_EMERG "setup_2\n");
	//初始化块设备I/O请求队列,这里是生成一个将自动接收上层传递过来的请求的队列空间.然后把队列固定传递给请求队列处理函数,也返回队列的指针
	bdev->queue = blk_init_queue(blk_request_process,&ramblock_lock);
printk(KERN_EMERG "setup_3\n");
	blk_queue_logical_block_size(bdev->queue,sect_size);	//设置扇区大小
printk(KERN_EMERG "setup_4\n");
	bdev->gd = alloc_disk(8);				//分配通用块设备描述结构,分配次设备号 2个
printk(KERN_EMERG "setup_5\n");
	bdev->gd->major = major;				//填写主设备号,但并未关联到主设备号上	!!!CCCC!!!
	bdev->gd->first_minor = 0;				//设置,第一个次设备号,由此可推算其他次设备的设备号
	bdev->gd->fops = &bdevops;				//设置,操作函数集
	bdev->gd->queue = bdev->queue;				//设置,请求队列
	bdev->gd->private_data = bdev->queue;			//设置,私有数据,这里直接指向了bdev描述结构
printk(KERN_EMERG "setup_6\n");
	sprintf(bdev->gd->disk_name,"samp_blk");		//设置,disk_name,这里预示着快设备驱动有可能自动生成逻辑设备文件并映射
printk(KERN_EMERG "setup_7\n");
	set_capacity(bdev->gd, numsect);			//设置每个次设备的扇区数目
printk(KERN_EMERG "setup_8\n");
	add_disk(bdev->gd);					//把通用块设备描述符关联到主设备号上完成挂载设备驱动到linux上
								//并自动创建逻辑设备文件并映射
								//至此完成了快设备驱动的挂载
printk(KERN_EMERG "setup_9\n");
#endif

/*******************************************************************/

#if 0
	//对快设备驱动进行初始化
	bdev = kmalloc(sizeof(struct blk_dev),GFP_KERNEL);	//内核态空间分配一段内存空间保存一个 struct blk_dev 变量	
printk(KERN_EMERG "setup_1\n");
printk(KERN_EMERG "setup_2\n");
printk(KERN_EMERG "setup_3\n");
	bdev->gd = alloc_disk(8);				//分配通用块设备描述结构,分配次设备号 2个
printk(KERN_EMERG "setup_4\n");
	//初始化块设备I/O请求队列,这里是生成一个将自动接收上层传递过来的请求的队列空间.然后把队列固定传递给请求队列处理函数,也返回队列的指针
	bdev->queue = blk_init_queue(blk_request_process,&ramblock_lock);
	//blk_queue_logical_block_size(bdev->queue,sect_size);	//设置扇区大小
	bdev->gd->queue = bdev->queue;				//设置,请求队列
printk(KERN_EMERG "setup_4\n");
	major = register_blkdev(72,"sampleblock");
printk(KERN_EMERG "setup_5\n");
	bdev->gd->major = 72;				//填写主设备号,但并未关联到主设备号上
	bdev->gd->first_minor = 0;				//设置,第一个次设备号,由此可推算其他次设备的设备号
	sprintf(bdev->gd->disk_name,"samp_blk");		//设置,disk_name,这里预示着快设备驱动有可能自动生成逻辑设备文件并映射
	bdev->gd->fops = &bdevops;				//设置,操作函数集
	//bdev->gd->private_data = bdev->queue;			//设置,私有数据,这里直接指向了bdev描述结构
printk(KERN_EMERG "setup_6\n");
	set_capacity(bdev->gd, numsect);			//设置每个次设备的扇区数目
printk(KERN_EMERG "setup_7\n");
	//分配一段用户空间的内存来,模拟一个硬盘
	bdev->size = numsect * sect_size;
	//bdev->data = vmalloc(bdev->size);
	bdev->data = kzalloc(bdev->size,GFP_KERNEL);
printk(KERN_EMERG "setup_8\n");
	add_disk(bdev->gd);					//把通用块设备描述符关联到主设备号上完成挂载设备驱动到linux上
printk(KERN_EMERG "setup_9\n");
#endif

	printk(KERN_EMERG "sampleblock_install\n");
	return 0;
}

static int blkdev_exit(void)
{
#if 1
	del_gendisk(bdev->gd);		//删除通用块设备描述符
printk(KERN_EMERG "exit_1\n");
	blk_cleanup_queue(bdev->queue);		//删除请求队列
printk(KERN_EMERG "exit_2\n");
	//vfree(bdev->data);
        kfree(bdev->data);
printk(KERN_EMERG "exit_3\n");
#endif
	unregister_blkdev(72,"sampleblock");	//注销主设备号,就是释放从linux申请来的资源,另外还会自动删除逻辑设备文件.
						//至此已把设备从linux上卸载掉
printk(KERN_EMERG "exit_4\n");
	kfree(bdev);
printk(KERN_WARNING "exit_5\n");

	printk(KERN_EMERG"sampleblock_release\n");
	return 0;
}

MODULE_LICENSE("GPL");

module_init(blkdev_init);
module_exit(blkdev_exit);
