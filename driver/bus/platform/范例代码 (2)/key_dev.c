#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");

#define GPFCON 0x56000050

static struct resource key_resource[] = {
	[0] = {
		.start = GPFCON,
		.end   = GPFCON + 8,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_EINT0,
		.end   = IRQ_EINT2,
		.flags = IORESOURCE_IRQ,
	},
};

struct platform_device key_device = {
	.name		  = "my-key",
	.id		  = 0,
	.num_resources	  = ARRAY_SIZE(key_resource),
	.resource	  = key_resource,
};

static int button_init()
{
    platform_device_register(&key_device);
     
    return 0;
}


static void button_exit()
{	   
    platform_device_unregister(&key_device);
}


module_init(button_init);
module_exit(button_exit);