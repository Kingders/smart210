
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

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
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h> /* invalidate_bdev */
#include <linux/bio.h>

MODULE_LICENSE("Dual BSD/GPL");

static int major = 0;

static int sect_size = 512;

static int nsectors = 1024; 

/*
* The internal representation of our device.
*/
struct blk_dev{
         int size;                        /* Device size in sectors */
         u8 *data;                        /* The data array */
         struct request_queue *queue;     /* The device request queue */
         struct gendisk *gd;              /* The gendisk structure */
};

struct blk_dev *dev;


/*
* Handle an I/O request, in sectors.
*/
static void blk_transfer(struct blk_dev *dev, unsigned long sector,
   unsigned long nsect, char *buffer, int write)
{
unsigned long offset = sector*sect_size;
unsigned long nbytes = nsect*sect_size;

if ((offset + nbytes) > dev->size) {
   printk (KERN_NOTICE "Beyond-end write (%ld %ld)\n", offset, nbytes);
   return;
}
if (write)
   memcpy(dev->data + offset, buffer, nbytes);
else
   memcpy(buffer, dev->data + offset, nbytes);
}

/*
* The simple form of the request function.
*/
static void blk_request(struct request_queue *q)
{
struct request *req;

req = blk_fetch_request(q);
while (req != NULL) {
   struct blk_dev *dev = req->rq_disk->private_data;

   blk_transfer(dev, blk_rq_pos(req), blk_rq_cur_sectors(req), req->buffer, rq_data_dir(req));
   
   if(!__blk_end_request_cur(req, 0)) 
   {
    req = blk_fetch_request(q);
   }
}
}


/*
* Transfer a single BIO.
*/
static int blk_xfer_bio(struct blk_dev *dev, struct bio *bio)
{
int i;
struct bio_vec *bvec;
sector_t sector = bio->bi_sector;

/* Do each segment independently. */
bio_for_each_segment(bvec, bio, i) {
   char *buffer = __bio_kmap_atomic(bio, i, KM_USER0);
   blk_transfer(dev, sector, bio_cur_bytes(bio)>>9 /* in sectors */, 
    buffer, bio_data_dir(bio) == WRITE);
   sector += bio_cur_bytes(bio)>>9; /* in sectors */
   __bio_kunmap_atomic(bio, KM_USER0);
}
return 0; /* Always "succeed" */
}

/*
* Transfer a full request.
*/
static int blk_xfer_request(struct blk_dev *dev, struct request *req)
{
struct bio *bio;
int nsect = 0;
     
__rq_for_each_bio(bio, req) {
   blk_xfer_bio(dev, bio);
   nsect += bio->bi_size/sect_size;
}
return nsect;
}



/*
* The device operations structure.
*/
static struct block_device_operations blk_ops = {
.owner            = THIS_MODULE,
};


/*
* Set up our internal device.
*/
static void setup_device()
{
/*
* Get some memory.
*/
dev->size = nsectors*sect_size;
dev->data = vmalloc(dev->size);
if (dev->data == NULL) {
   printk (KERN_NOTICE "vmalloc failure.\n");
   return;
}

   dev->queue = blk_init_queue(blk_request, NULL);
   if (dev->queue == NULL)
    goto out_vfree;

blk_queue_logical_block_size(dev->queue, sect_size);
dev->queue->queuedata = dev;
/*
* And the gendisk structure.
*/
dev->gd = alloc_disk(1);
if (! dev->gd) {
   printk (KERN_NOTICE "alloc_disk failure\n");
   goto out_vfree;
}
dev->gd->major = major;
dev->gd->first_minor = 0;
dev->gd->fops = &blk_ops;
dev->gd->queue = dev->queue;
dev->gd->private_data = dev;
sprintf (dev->gd->disk_name, "simp_blk%d", 0);
set_capacity(dev->gd, nsectors*(sect_size/sect_size));
add_disk(dev->gd);
return;

out_vfree:
if (dev->data)
   vfree(dev->data);
}

static int __init blk_init(void)
{
/*
* Get registered.
*/
major = register_blkdev(major, "sbdblk");
if (major <= 0) {
   printk(KERN_WARNING "blk: unable to get major number\n");
   return -EBUSY;
}

dev = kmalloc(sizeof(struct blk_dev), GFP_KERNEL);
if (dev == NULL)
   goto out_unregister;

   setup_device();
     
return 0;

out_unregister:
unregister_blkdev(major, "sbdblk");
return -ENOMEM;
}

static void blk_exit(void)
{

   if (dev->gd) {
    del_gendisk(dev->gd);
    put_disk(dev->gd);
   }
   if (dev->queue)
    blk_cleanup_queue(dev->queue);
   if (dev->data)
    vfree(dev->data);

   unregister_blkdev(major, "sbdblk");
kfree(dev);
}

module_init(blk_init);
module_exit(blk_exit);
