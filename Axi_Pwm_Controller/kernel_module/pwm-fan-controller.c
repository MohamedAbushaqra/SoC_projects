#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/interrupt.h>

#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/cdev.h>

#define DRIVER_NAME "PWM_FAN_CONTROLLER"

// --------------------- IOCTL Strings ----------------------------------- //
#define IOCTL_DEVICE_DRIVER		'r'
#define IOCTL_PWM_SPEED_FULLSPEED	_IOWR (IOCTL_DEVICE_DRIVER, 0, int)
#define IOCTL_PWM_SPEED_MIDDLE		_IOWR (IOCTL_DEVICE_DRIVER, 1, int)
#define IOCTL_PWM_SPEED_QUIET		_IOWR (IOCTL_DEVICE_DRIVER, 2, int)
#define IOCTL_PWM_SPEED_OFF			_IOWR (IOCTL_DEVICE_DRIVER, 4, int)
#define IOCTL_PWM_SPEED_CUSTOM		_IOWR (IOCTL_DEVICE_DRIVER, 5, int)


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mohamed Abushaqra");
MODULE_DESCRIPTION("simple loadable module to test pwm fan controller axi lite ip core");

struct pwm_fan_controller_local {
	unsigned long mem_start;
	unsigned long mem_end;
	void __iomem *base_addr;

	struct device *device;
	struct cdev cdev;
	dev_t devid;
	struct class *class;
	void *private_data;
};


static int pwm_fan_control_open(struct inode *inode, struct file *file)  {

	struct pwm_fan_controller_local *lp;
	lp = container_of(inode->i_cdev, struct pwm_fan_controller_local, cdev);
	file->private_data = lp;

	return 0;
}


static long pwm_fan_control_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	struct pwm_fan_controller_local *lp = filep->private_data;
	unsigned int tempVal;

	switch (cmd)
    {
		case IOCTL_PWM_SPEED_FULLSPEED:
			iowrite32( 0xff, lp->base_addr ); // Active low configuration
		break;

		case IOCTL_PWM_SPEED_MIDDLE:
			iowrite32( 0x96, lp->base_addr );
		break;

		case IOCTL_PWM_SPEED_QUIET:
			iowrite32( 0x0F, lp->base_addr );
		break;

		case IOCTL_PWM_SPEED_OFF:
			iowrite32( 0x00, lp->base_addr );
		break;

		case IOCTL_PWM_SPEED_CUSTOM:
			if ((arg > 255) || (arg < 0 ))
			{
				printk("Unable to set the given value, please choose between 0 and 255.\n");
			}
			else
			{
				printk("Setting the duty cycle register to %d\n", arg);
				iowrite32(arg, lp->base_addr);
			}
		break;
	}
	return 0;
}


static const struct file_operations driver_fops = {
	.owner = THIS_MODULE,
	.open  = pwm_fan_control_open,
	.unlocked_ioctl = pwm_fan_control_ioctl,
};

static int pwm_fan_control_probe(struct platform_device *pdev)
{
	struct resource *r_mem; /* IO mem resources */
	struct device *dev = &pdev->dev;
	struct pwm_fan_controller_local *lp = NULL;

	int rc = 0;
	dev_info(dev, "Device Tree Probing\n");
	r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r_mem)
	{
		dev_err(dev, "invalid address\n");
		return -ENODEV;
	}

	lp = (struct pwm_fan_controller_local *) kmalloc(sizeof(struct pwm_fan_controller_local), GFP_KERNEL);
	if (!lp)
	{
		dev_err(dev, "Cound not allocate PWM_FAN_CONTROLLER device\n");
		return -ENOMEM;
	}

	dev_set_drvdata(dev, lp);
	lp->mem_start = r_mem->start;
	lp->mem_end = r_mem->end;

	if (!request_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1, DRIVER_NAME))
	{
		dev_err(dev, "Couldn't lock memory region at %p\n", (void *)lp->mem_start);
		rc = -EBUSY;
		goto error1;
	}

	lp->base_addr = ioremap(lp->mem_start, lp->mem_end - lp->mem_start + 1);
	if (!lp->base_addr)
	{
		dev_err(dev, "PWM_FAN_CONTROLLER: Could not allocate iomem\n");
		rc = -EIO;
		goto error2;
	}

	dev_info(dev,"PWM_FAN_CONTROLLER at 0x%08x mapped to 0x%08x \n",
		(unsigned int __force)lp->mem_start,
		(unsigned int __force)lp->base_addr);


	alloc_chrdev_region(&lp->devid, 0, 1, DRIVER_NAME);
    cdev_init(&lp->cdev, &driver_fops);
    rc = cdev_add(&lp->cdev, lp->devid, 1);

	if (rc) {
		printk ("cdev_add() failed\n");
		goto error2;
	}

    lp->class = class_create(THIS_MODULE, DRIVER_NAME);
    if(IS_ERR(lp->class))
    {
        return PTR_ERR(lp->class);
    }

    lp->device = device_create(lp->class, NULL, lp->devid, NULL, DRIVER_NAME);
    if(IS_ERR(lp->device))
    {
        return PTR_ERR(lp->device);
    }

	printk("Char device %s can be accessed through /dev! \n", DRIVER_NAME);

	return 0;


	error2:
		release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
	error1:
		kfree(lp);
		dev_set_drvdata(dev, NULL);
		return rc;
}

static int pwm_fan_control_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct pwm_fan_controller_local *lp = dev_get_drvdata(dev);
	iounmap(lp->base_addr);
	release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);

	cdev_del(&lp->cdev);
	device_destroy(lp->class, lp->devid);
	class_destroy(lp->class);
	kfree(lp);
	dev_set_drvdata(dev, NULL);
	return 0;
}

static struct of_device_id pwm_fan_control_of_match[] = {
	{ .compatible = "xlnx,pwm-controller-1.0", },
	{ /* end of list */ },
};
MODULE_DEVICE_TABLE(of, pwm_fan_control_of_match);

static struct platform_driver pwm_fan_control_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table	= pwm_fan_control_of_match,
	},
	.probe		= pwm_fan_control_probe,
	.remove		= pwm_fan_control_remove,
};

static int __init pwm_fan_control_init(void)
{
	return platform_driver_register(&pwm_fan_control_driver);
}


static void __exit pwm_fan_control_exit(void)
{
	platform_driver_unregister(&pwm_fan_control_driver);
	printk(KERN_ALERT "Goodbye module world.\n");
}

module_init(pwm_fan_control_init);
module_exit(pwm_fan_control_exit);
