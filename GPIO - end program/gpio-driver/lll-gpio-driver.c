#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/proc_fs.h>
#include <linux/slab.h>

#include <asm/io.h>

#define LLL_MAX_USER_SIZE 1024

#define BCM2837_GPIO_ADDRESS 0x3F200000

static char data_buffer[LLL_MAX_USER_SIZE+1] = {0};

static unsigned int *gpio_registers = NULL;

//special functioins init and exit - module is insatlled  when driver is installed and exits
//printk is kernel system buffer - dmesg 
//then at the bottom we have metadata

// proc_dir_entry is a global variable 
//static struct means that it is own visibke inside this unit i.e. the file

static struct proc_dir_entry *lll_proc = NULL;

static void gpio_pin_on(unsigned int pin)
{	
	//so we index into GPIO registers structure - so the function index is the pin no/10
	unsigned int fsel_index = pin/10;
	//the function select bit position os pin mod 10
	//function select register is  something that chnages the value of 
	//a register 
	unsigned int fsel_bitpos = pin%10;
	//get the address of that register and add index to the poiunter value - address that makes pin output
	unsigned int* gpio_fsel = gpio_registers + fsel_index;
	//address of register to turn pin on 
	unsigned int* gpio_on_register = (unsigned int*)((char*)gpio_registers + 0x1c);
	
	//bitwise math going on - to cchange all to inputs then swithces on last value to input - to clear all prebious values
	*gpio_fsel &= ~(7 << (fsel_bitpos*3));
	*gpio_fsel |= (1 << (fsel_bitpos*3));
	*gpio_on_register |= (1 << pin);

	return;
}

static void gpio_pin_off(unsigned int pin)
{	
	
	unsigned int *gpio_off_register = (unsigned int*)((char*)gpio_registers + 0x28);
	*gpio_off_register |= (1<<pin);
	return;
}

//read takes in a buffer from the user, size and then an offset pointer into how much is already read
//to copy to user space - copies data from kernel space to user space
//ssize_t is a signed type - able to repsent the number -1 which is important fo system calls
//loff_t - long offset that seeks users position in current file
ssize_t lll_read(struct file *file, char __user *user, size_t size, loff_t *off) {

	return copy_to_user(user,"Hello!\n", 7) ? 0 : 7;

}

//so the writing function here we want to use the procfs function to send a value i.e. the specific pin and then a 1 or 0 to switch it high or low
//write to procfs file, user buffer, size of user is writing 

ssize_t lll_write(struct file *file, const char __user *user, size_t size, loff_t *off)
{
	unsigned int pin = UINT_MAX;
	unsigned int value = UINT_MAX;

	//everytime the user writes then we delete the old data to replace the new stuff
        memset(data_buffer, 0x0, sizeof(data_buffer));
        //if the data recieved has a size larger than th max buffer size - then we make the n we cap off how much is given to prevent over runs
        //this could be looped to continuosly stream data?

	if (size > LLL_MAX_USER_SIZE)
	{
		size = LLL_MAX_USER_SIZE;
	}

	//So now we need to parse the value and gather infromation from it
	if (copy_from_user(data_buffer, user, size))
		return 0;

	printk("Data buffer: %s\n", data_buffer);
	
	//use scanf to grab data from the buffer and then extract the pin required to change and then also high or low (0/1)
	if (sscanf(data_buffer, "%d,%d", &pin, &value) != 2)
	{
		printk("Inproper data format submitted\n");
		return size;
	}

	//only have access to pin21 - 0 so want to check for these values

	if (pin > 21 || pin < 0)
	{
		printk("Invalid pin number submitted\n");
		return size;
	}
	//same with value - only want 0 or 1
	if (value != 0 && value != 1)
	{
		printk("Invalid on/off value\n");
		return size;
	}
	//then we trun the gpio pins on and off using our 
	printk("You said pin %d, value %d\n", pin, value);
	if (value == 1)
	{
		gpio_pin_on(pin);
	} else if (value == 0)
	{
		gpio_pin_off(pin);
	}

	return size;
}


//before linux kernel 5.4 - you use a proc_fs structure - so when we have a procfs file we use this structure to define what is happen when its read to or wrote to

static const struct proc_ops lll_proc_fops = 
{
	.proc_read = lll_read,
	.proc_write = lll_write,
};

static int __init gpio_driver_init(void)

{
	printk("Welcome to my driver!\n");

	//So first we have to map in the GPIO address to the interface, GPIO lives at a physical address - needs mapping in - kernel cannot access by default
	//Every address goes through a MMU (memroy management unit) - job of this to turn virtual address to the physical address 
	//if mmu cannot get a physical address from the virtual address it will fail - in our case it is -  0x3F200000
	
	gpio_registers = (int*)ioremap(BCM2837_GPIO_ADDRESS, PAGE_SIZE);
	if (gpio_registers == NULL)
	{
		printk("Failed to map GPIO memory to driver\n");
		return -1;
	}
	
	printk("Successfully mapped in GPIO memory\n");

	//create an entry into proc-fs - specify name, 666 is a permission , let anyone use it, NULL is a set of flags that we don't want to have any value
	//&lll_proc_fops the address of the lll proc-fops system - this is written for after linux  kernel 5.4
	lll_proc = proc_create("lll-gpio", 0666, NULL, &lll_proc_fops);
	if (lll_proc == NULL)
	{
		return -1;
	}
	return 0;

}

static void __exit gpio_driver_exit(void)
{
	printk("Leaving  my driver!\n");
	proc_remove(lll_proc);
        return;
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Low Level Learning");
MODULE_DESCRIPTION("Test of writing drivers for RASPI");
MODULE_VERSION("1.0");
