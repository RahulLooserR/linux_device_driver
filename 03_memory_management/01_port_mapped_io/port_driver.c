/* ************************************************************************
 * Created by    : Rahul Kumar Nonia
 * File name     : port_driver.c
 * Created on    : Saturday 05 December 2020 01:10:11 PM IST
 * Last modified : Saturday 05 December 2020 01:28:58 PM IST
 * Description   : device driver for port io 
 * ***********************************************************************/

#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/init.h>


// free address should be available (/proc/ioport)
#define IOSTART 0x200
#define IOEXTEND 0x40

static unsigned long iostart = IOSTART, ioextend = IOEXTEND, ioend;

static int __init my_ioport_init(void)
{
	unsigned long ul_test = (unsigned long)100;
	ioend = iostart + ioextend;

	pr_info("requesting the io region 0x%lx tpo 0x%lx\n", iostart, ioend);

	if(!request_region(iostart, ioextend, "my_ioport")){
		pr_info("Io region is busy, exiting ...!\n");
		return -EBUSY;
	}

	pr_info("writing a long data = %ld\n", ul_test);
	outl(ul_test, iostart);

	ul_test = inl(iostart);

	pr_info("reading a long data = %ld\n", ul_test);

	return 0;
}

static void __exit my_ioport_exit(void)
{
	pr_info("releasing the io region 0x%lx tpo 0x%lx\n", iostart, ioend);
	release_region(iostart, ioextend);
	
}

module_init(my_ioport_init);
module_exit(my_ioport_exit);

MODULE_AUTHOR("new author");
MODULE_DESCRIPTION("IO port demo, requesting io ports");
MODULE_LICENSE("GPL");
