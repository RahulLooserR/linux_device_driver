/* ************************************************************************
 * Created by    : Rahul Kumar Nonia
 * File name     : simple_char_driver.c
 * Created on    : Saturday 24 October 2020 10:15:14 PM IST
 * Last modified : Monday 26 October 2020 07:09:52 AM IST
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
#include <linux/ioctl.h>
#include <linux/proc_fs.h>

#define SIZE 256
#define DEVICE_NAME "my_char_device"
#define PROC_NAME "chr_proc"
#define MODE 0666
#define DEVICE_CLASS "my_char_device_class"

/* define  ioctl code */
#define WR_DATA _IOW('a','a',int32_t*)
#define RD_DATA _IOR('a','b',int32_t*)
int32_t val = 0;

static dev_t dev_num = 0; /* used for major/minor number */
static struct class *dev_class;
static struct cdev my_cdev;

static char *kernel_buf;
static char arr[SIZE] = "this is for proc filesystem\n";
static int len = 1;

/* Entry  point for driver */
static int __init chr_dev_init(void);

/* Exit point */
static void __exit chr_dev_exit(void);

/* **** proc operations prototype **** */
static ssize_t proc_read (struct file *, char __user *, size_t, loff_t *);
static ssize_t proc_write (struct file *, const char __user *, size_t, loff_t *);
static int proc_open (struct inode *, struct file *);
static int proc_release (struct inode *, struct file *);

static struct file_operations proc_ops = 
{
	.open 		= proc_open,
	.read		= proc_read,
	.write 		= proc_write,
	.release	= proc_release,
};
/******************* Ending proc prototype *************/


/* file operations protoytpe */
static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *fileptr, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *fileptr, const char __user *buf, size_t len, loff_t *off);
static long my_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);

static struct file_operations fops = 
{
	.owner			= THIS_MODULE,
	.read			= my_read,
	.open			= my_open,
	.write			= my_write,
	.unlocked_ioctl = my_ioctl,
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

static long my_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	switch(cmd){
		case RD_DATA:
			copy_from_user(&val, (int32_t*)arg, sizeof(val));
			pr_info("data copied/received from user: %d\n", val);
			break;

		case WR_DATA:
			copy_to_user((int32_t*)arg, &val, sizeof(val));
			pr_info("Data sent to user: %d\n", val);
			break;

		default:
			break;
	}
	return 0;
}

static int proc_open (struct inode *inode, struct file *file)
{
	pr_info("open for proc\n");
	return 0;
}

static int proc_release (struct inode *inode, struct file *file)
{
	pr_info("release for proc\n");
	return 0;
}

static ssize_t proc_read (struct file *file, char __user *buf, size_t length, loff_t *loff)
{
	if(len){
		len = 0;
		copy_to_user(buf, arr, SIZE);
	}
	else{
		len = 1;
		return 0;
	}
	pr_info("proc read called: %s\n", arr);
	return length;
}

static ssize_t proc_write (struct file *file, const char __user *buf, size_t length, loff_t *loff)
{
	copy_from_user(arr, buf, length);
	pr_info("proc write: %s\n", arr);
	len = 1;
	return length;
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

	/* creating proc entry */
	proc_create(PROC_NAME, MODE, NULL, &proc_ops);

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
