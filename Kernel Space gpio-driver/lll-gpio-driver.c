#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

//special functioins init and exit - module is insatlled  when driver is installed and exits
//printk is kernel system buffer - dmesg 
//then at the bottom we have metadata

static int __init gpio_driver_init(void)

{
	printk("Welcome to my driver!\n");
	return 0;

}

static void __exit gpio_driver_exit(void)
{
	printk("Leaving  my driver!\n");
        return;
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Low Level Learning");
MODULE_DESCRIPTION("Test of writing drivers for RASPI");
MODULE_VERSION("1.0");
