/* ************************************************************************
 * Created by    : Rahul Kumar Nonia
 * File name     : simple_char_driver.c
 * Created on    : Saturday 24 October 2020 10:15:14 PM IST
 * Last modified : Sunday 25 October 2020 05:08:03 PM IST
 * Description   : 
 * ***********************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

#define SIZE 256
#define DEVICE_NAME "my_char_device"
#define DEVICE_CLASS "my_char_device_class"

static dev_t dev_num = 0; /* used for major/minor number */
static struct class *dev_class;
static struct cdev my_cdev;

static char *kernel_buf;

/* Entry  point for driver */
static int __init chr_dev_init(void);

/* Exit point */
static void __exit chr_dev_exit(void);

/* file operations protoytpe */
static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *fileptr, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *fileptr, const char __user *buf, size_t len, loff_t *off);

static struct file_operations fops = 
{
	.owner			= THIS_MODULE,
	.read			= my_read,
	.open			= my_open,
	.write			= my_write,
	.release		= my_release,
};

static int my_open(struct inode *inode, struct file *file)
{
	/* creating physical memory using kmalloc */
	if((kernel_buf = (char *)kmalloc(SIZE, GFP_KERNEL)) == 0){
		pr_info("cannot allocate memory\n");
		return -1;
	}
	pr_info("Device file opened\n");
	return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
	kfree(kernel_buf);
	pr_info("device closed, memory released\n");
	return 0;
}

static ssize_t my_read(struct file *fileptr, char __user *buf, size_t len, loff_t *off)
{
	copy_to_user(buf, kernel_buf, SIZE);
	pr_info("Data read done\n");
	return SIZE;
}

static ssize_t my_write(struct file *fileptr, const char __user *buf, size_t len, loff_t *off)
{
	copy_from_user(kernel_buf, buf, len);
	pr_info("Data written successfully\n");
	return len;
}

static int __init chr_dev_init(void)
{
	/* allocating statically major number 
	 * int register_chrdev_region(dev_t first, unsigned int count, char *name);
	 */

	/* Allocating major number dynamically */
	if(alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME) < 0){
		/* old method of printing */
		printk(KERN_INFO "cannot allocate major number\n");
		return -1;
	}
	/* printing with same log level */
	pr_info("allocating major number succeeded\n");
	
	pr_info("Major no: %d, minor no:  %d\n", MAJOR(dev_num), MINOR(dev_num));

	/* create struct class */
	if((dev_class = class_create(THIS_MODULE, DEVICE_CLASS)) == NULL){
		pr_info("Cannot create the struct class \n");
		goto r_class;
	}
	
	/* creating cdev structure */
	cdev_init(&my_cdev, &fops);

	/* adding character device to the system */
	if(cdev_add(&my_cdev, dev_num, 1) < 0){
		pr_info("cannot  add device to the system\n");
		goto r_class;
	}

	/* creating device */
	if(device_create(dev_class, NULL, dev_num, NULL, DEVICE_NAME) == NULL){
		pr_info("cannot create device\n");
		goto r_device;
	}
	pr_info("device driver inserted properly\n");

	return 0;

	/* goto fallback */
r_class:
	unregister_chrdev_region(dev_num, 1);
	return -1;

r_device:
	class_destroy(dev_class);
	return -1;
}

static void __exit chr_dev_exit(void)
{
	device_destroy(dev_class, dev_num);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev_num, 1);
	pr_info("device driver removed successfully\n");
}

module_init(chr_dev_init);
module_exit(chr_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rahul has created this demo driver");
MODULE_DESCRIPTION("this is the demostration of pseudo char device");
