/* ************************************************************************
 * Created by    : Rahul Kumar Nonia
 * File name     : helloworld.c
 * Created on    : Saturday 24 October 2020 06:19:12 PM IST
 * Last modified : Saturday 24 October 2020 06:33:09 PM IST
 * Description   : demo on linux device driver 
 * ***********************************************************************/
	
#include <linux/module.h>


/* always declare  global variable/function static */
static int x = 0;

/* drivers entry point */
static int __init hello_init(void)
{
	printk (KERN_INFO "This is hello world demo: %d\n", x + 5);
	return 0;
}

/* drivers exit point */
void __exit hello_exit(void)
{
	printk (KERN_INFO "This is hello world exit\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
