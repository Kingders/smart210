#include "linux/module.h"
#include "linux/errno.h"
#include "linux/interrupt.h"
#include "linux/mm.h"
#include "linux/fs.h"
#include "linux/kernel.h"
#include "linux/timer.h"
#include "linux/genhd.h"
#include "linux/hdreg.h"
#include "linux/ioport.h"
#include "linux/init.h"
#include "linux/wait.h"
#include "linux/blkdev.h"
#include "linux/blkpg.h"
#include "linux/delay.h"
#include "linux/io.h"
 
//#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/dma.h>
 
static struct gendisk *ramblock_disk;
static struct request_queue *ramblock_queue;
 
static int major;
 
static DEFINE_SPINLOCK(ramblock_lock);
 
#define RAMBLOCK_SIZE (1024*1024)
static unsigned char *ramblock_buf;
 
static int ramblock_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
        // 容量=heads*cylinders*sectors*512 
        geo->heads     = 2;
        geo->cylinders = 32;
        geo->sectors   = RAMBLOCK_SIZE/2/32/512;
        return 0;
}
 
 
static struct block_device_operations ramblock_fops = {
        .owner = THIS_MODULE,
        .getgeo = ramblock_getgeo,
};
 
static void do_ramblock_request(struct request_queue * q)
{

        static int r_cnt = 0;
        static int w_cnt = 0;
        struct request *req;
        //printk("do_ramblock_request %d\n", ++cnt);
	void *buffer;

	req = blk_fetch_request(q);
        while (req) {
#if 1
                // 数据传输三要素: 源,目的,长度 
                // 源/目的: 
                unsigned long offset = blk_rq_pos(req) * 512;
 
                // 目的/源: (写的时候buffer是源，读的时候buffer是目的，从扇区里读出来放在buffer里)
                // req->buffer
 
                // 长度:
                unsigned long len = blk_rq_cur_sectors(req) * 512;

		// 请求的buffer区域
		buffer = bio_data(req->bio);
 
                if (rq_data_dir(req) == READ)
                {      
                        //如果是操作硬盘的话在这个位置放置读取硬盘的函数就可以了
                        //printk("do_ramblock_request read %d\n", ++r_cnt);
                        memcpy(buffer, ramblock_buf+offset, len);
                }
                else
                {
                        //如果是操作硬盘的话在这个位置放置写硬盘的函数就可以了
                        //printk("do_ramblock_request write %d\n", ++w_cnt);
                        memcpy(ramblock_buf+offset, buffer, len);
                }
  #endif              
		if (!__blk_end_request_cur(req, 0))
			req = blk_fetch_request(q);
        }

}
 
static int ramblock_init(void)
{
        // 1. 分配一个gendisk结构体 
        ramblock_disk = alloc_disk(16); // 次设备号个数: 分区个数+1 ，表示有15个分区
 
        // 2. 设置 
        // 2.1 分配/设置队列: 提供读写能力 
        ramblock_queue = blk_init_queue(do_ramblock_request, &ramblock_lock);//do_ramblock_request队列处理函数
        ramblock_disk->queue = ramblock_queue;
        // 2.2 设置其他属性: 比如容量 
        major = register_blkdev(0, "ramblock");  // cat /proc/devices
        ramblock_disk->major       = major;
        ramblock_disk->first_minor = 0;
        sprintf(ramblock_disk->disk_name, "ramblock");
        ramblock_disk->fops        = &ramblock_fops;
        set_capacity(ramblock_disk, RAMBLOCK_SIZE / 512);   //在内核里面对于文件系统那一层，认为扇区永远是512字节，即为扇区数
                                                            //块设备的操作是以扇区为单位的。     
        // 3. 硬件相关操作 
        ramblock_buf = kzalloc(RAMBLOCK_SIZE, GFP_KERNEL);
 
        // 4. 注册 
        add_disk(ramblock_disk);
 
        return 0;
}
 
static void ramblock_exit(void)
{
        unregister_blkdev(major, "ramblock");
        del_gendisk(ramblock_disk);
        put_disk(ramblock_disk);
        blk_cleanup_queue(ramblock_queue);
 
        kfree(ramblock_buf);
}
 
module_init(ramblock_init);
module_exit(ramblock_exit);
 
MODULE_LICENSE("GPL");
