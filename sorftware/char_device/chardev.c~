/*************************************************************
(include/linux/fs.h)
struct file_operations {		//文件操作函数集
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);	//loff_t表示实际开始处与实际基地址间的偏移
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);	//loff_t表示实际开始处与实际基地址间的偏移
	int (*open) (struct inode *, struct file *);
	int (*release) (struct inode *, struct file *);
	....
struct inode {				//文件节点信息
	dev_t			i_rdev;	//主设备号,dev_t主要是有主设备号和某个次设备号共同构成的
	loff_t			i_size;
	...
struct file {				//文件描述符
	void			*private_data;		//私用数据的地址
	loff_t			f_pos;			//文件描述符记录起始位置的指针


fs/char_dev.c
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count,const char *name)	//动态申请设备号
int register_chrdev_region(dev_t from, unsigned count, const char *name)	//静态申请设备号
void unregister_chrdev_region(dev_t from, unsigned count)			//注销设备

(arch/ia64/include/asm/cacheflush.h)
#define copy_to_user_page(vma, page, vaddr, dst, src, len)
(arch/ia64/include/asm/uaccess.h)
#define copy_to_user(to, from, n)	//把数据从 设备驱动 拷贝到 用户进程
					//to:用户进程数据指针,from:设备驱动数据指针,n:要拷贝的数据量
(linux-source-3.13.0/arch/ia64/include/asm/cacheflush.h)
#define copy_from_user_page(vma, page, vaddr, dst, src, len) 
(linux-source-3.13.0/arch/ia64/include/asm/uaccess.h)
#define copy_from_user(to, from, n)	//把数据从 用户进程 拷贝到 设备驱动 
					//from:用户进程数据指针,to:设备驱动数据指针,n:要拷贝的数据量


(include/linux/cdev.h)
struct cdev {
	struct kobject kobj;
	struct module *owner;
	const struct file_operations *ops;
	struct list_head list;
	dev_t dev;		//主设备号,dev_t主要是有主设备号和第一个次设备号共同构成的
	unsigned int count;	//次设备个数
};
int cdev_add(struct cdev *, dev_t, unsigned);
void cdev_init(struct cdev *, const struct file_operations *);
void cdev_del(struct cdev *);



调试:
	1,read write 的	filep->f_pos += size ;	关于当前位置的变化的调整没有调整正确!!! 有机会再处理
	2,要求在root用户下使用 app 的read write程序,不然调试失败
	3,最后	make //编译得chardev.ko
		cat /proc/devices	//知道chardev 分配得主设备号243
		mknod /dev/chardev0 c 243 0	//创建逻辑设备文件映射设备驱动
		gcc app/read.c -o read
		gcc app/write.c -o write
		sudo ./write	//用户程序写测试
		sudo ./read	//用户程序读测试
	4,读操作追溯:read->SVC->查看得系统调用;sys_read->VFS_read->file_ops->cdev_read()
	5,打开操作追溯:open->~~~~->cdev_open,open完成创建文件描述符等操作,而传递到最后的cdev_open只需要专心完成硬件操作


编译信息:
kingders@kingders-ThinkPad-T420:~/smart210/sorftware/char_device$ make
make -C /usr/src/linux-headers-4.2.0-42-generic M=/home/kingders/smart210/sorftware/char_device modules
make[1]: Entering directory `/usr/src/linux-headers-4.2.0-42-generic'
  CC [M]  /home/kingders/smart210/sorftware/char_device/chardev.o
In file included from /home/kingders/smart210/sorftware/char_device/chardev.c:61:0:
/home/kingders/smart210/sorftware/char_device/chardev.c: In function ‘__exittest’:
include/linux/module.h:135:4: warning: return from incompatible pointer type [enabled by default]
  { return exitfn; }     \
    ^
/home/kingders/smart210/sorftware/char_device/chardev.c:190:1: note: in expansion of macro ‘module_exit’
 module_exit(chardev_exit);
 ^
  Building modules, stage 2.
  MODPOST 1 modules
  CC      /home/kingders/smart210/sorftware/char_device/chardev.mod.o
  LD [M]  /home/kingders/smart210/sorftware/char_device/chardev.ko
make[1]: Leaving directory `/usr/src/linux-headers-4.2.0-42-generic'




**************************************************************/


//linux模块加载使用的头文件
#include <linux/module.h>
#include <linux/init.h>
//字符设备驱动需要的头文件
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>




struct cdev chardev; //字符设备描述结构
dev_t devno;	//主设备号和第一个次设备号的合体数
int dev1_reg[5];	//模拟硬件设备的寄存器
int dev2_reg[5];	//模拟硬件设备的寄存器

loff_t cdev_lseek (struct file *filep, loff_t offset, int whence)	//filep:文件描述符,offset:偏移定位,whence起始位置
{									//变更当前位置
	loff_t	new_pos = 0;

	switch (whence)
	{
		case SEEK_SET:
			new_pos = 0 + offset;
			break;
		case SEEK_CUR:
			new_pos = filep->f_pos + offset;
			break;
		case SEEK_END:
			new_pos = 5*sizeof(int) + offset;
			break;	
	}
	//新位置出来后可以做些检查,这里忽略

	filep->f_pos = new_pos; 	//变更当前位置
	
	return new_pos;		//返回当前位置
}

ssize_t cdev_read (struct file *filep, char __user *buf, size_t size, loff_t *ppos)
{
	int *reg_base = filep->private_data;		//硬件寄存器基地址

	//中间应该还有对参数的检查,这里忽略了,不做

	copy_to_user(buf,reg_base+(*ppos),size);	//把数据从 设备驱动 拷贝到 用户进程
							//从opps开始

	filep->f_pos += size;				//变更当前位置

	return size;
}

ssize_t cdev_write (struct file *filep, const char __user *buf, size_t size, loff_t *ppos)
{
	int *reg_base = filep->private_data;		//硬件寄存器基地址

	//中间应该还有对参数的检查,这里忽略了,不做

	copy_from_user(reg_base+(*ppos),buf,size);	//把数据从 用户进程 拷贝到 设备驱动
							//从opps开始

	filep->f_pos += size;				//变更当前位置

	return size;
}

int cdev_open (struct inode *node, struct file *filep)	//inode:文件节点信息,filep;文件描述符
{	
	//主要是打开硬件,对其寄存器的操作.(然而这是测试程序,并没有对硬件的操作)
	//inode:每个linux上的文件都有一个唯一接inode储存文件的属性信息,逻辑设备文件(/dev/里的文件)也不例外.
	
	int min = MINOR(node->i_rdev);	//提取次设备号,MINOR是宏
	
	if( min == 0 )			//如果次设备号是0,打开次设备0的硬件
	{
		filep->private_data = dev1_reg;
	}
	if( min == 1 )			//如果次设备号是1,打开次设备0的硬件
	{
		filep->private_data = dev2_reg;
	}

	return 0;
}

int cdev_close (struct inode *node, struct file *filep)
{
	//主要是关闭硬件设备,(然而这是测试程序,并没有对硬件的实际操作)
	return 0;
}

struct file_operations	cdevops =
{
	.llseek = cdev_lseek,
	.read = cdev_read,
	.write = cdev_write,
	.open = cdev_open,
	.release = cdev_close,
};

static int chardev_init(void)
{
	cdev_init(&chardev, &cdevops);	//初始化cdev结构,

	alloc_chrdev_region(&devno, 0, 2,"chardev");	//动态分配主设备号
							//动态分配到的主设备号和第一个次设备号合体存于devno里,
							//起始次设备号为0,可以有两个次设备,设备名字chardev
							//主设备号是内核资源,这里完成设备驱动在内核上的登记,
							//至此cat /proc/devices已经可以查看到设备信息.

	cdev_add(&chardev, devno, 2);		//把主设备号和次设备个数填写到cdev里,并把设备描述结构关联到设备号上完成挂载设备驱动到linux上
						//至此完成cdev加载到linux的初始化操作. 
						//然而只挂载好了驱动,并没有自动创建逻辑设备文件映射,需要另外人工创建,
						//例 mknod /dev/chardevX XX XX 
	return 0;
}

static int chardev_exit(void)
{

	cdev_del(&chardev);			//删除结构,但申请的linux资源如设备号还没有注销,所以还可以读取到设备一些属性信息
	unregister_chrdev_region(devno, 2);	//注销主设备号,和分配的2个次设备,即完成从内核上注销一个设备
						//在/dev/里的映射的逻辑设备文件有可能会自动删掉
						//至此设备驱动已经完全从内核卸载掉.
	return 0;
}

MODULE_LICENSE("GPL");

module_init(chardev_init);
module_exit(chardev_exit);
