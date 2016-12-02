#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

extern struct bus_type key_bus_type;

struct device key1_dev = {
     .init_name = "key1_dev",
     .bus = &key_bus_type,	
};

int key1_device_init(void)
{
     int ret;
     ret = device_register(&key1_dev);		// 注册key1_dev信息到 总线 key_bus_type
     return ret;
     
}


int key1_device_exit(void)
{
	device_unregister(&key1_dev);		//从总线key_bus_type 卸载设备信息
     return 0;
}

module_init(key1_device_init);
module_exit(key1_device_exit);
