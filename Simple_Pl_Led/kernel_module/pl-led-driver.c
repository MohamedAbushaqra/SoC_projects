/*  pl-led-driver.c
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/interrupt.h>

#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/cdev.h>
#include <linux/of_platform.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR
    ("Mohamed Abushaqra");
MODULE_DESCRIPTION
    ("pl-led-driver, simple loadable module generated bz petalinux and modified by me to control PL leds");

#define DRIVER_NAME "pl-led-driver"
#define DEV_CNT  1


#define		Reg0_LED_1		0x00
#define		Reg0_LED_2		0x01
#define		Reg0_LED_3		0x02
#define		Reg0_LED_4		0x03

// --------------------- IOCTL Strings ----------------------------------- //

#define IOCTL_DEVICE_DRIVER		'r'
#define IOCTL_LEDS_ON			_IOWR (IOCTL_DEVICE_DRIVER, 0, int)
#define IOCTL_LEDS_OFF			_IOWR (IOCTL_DEVICE_DRIVER, 1, int)


struct pl_led_driver_local {
	int irq;
	unsigned long mem_start;
	unsigned long mem_end;
	void __iomem *base_addr;

	dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    int major;
    void *private_data;
};


static long led_control_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	struct pl_led_driver_local *lp = filep->private_data;
	unsigned int tempVal;

	switch (cmd)
    {
		case IOCTL_LEDS_ON:
			iowrite32( 0x1, lp->base_addr + Reg0_LED_1 );
		break;

		case IOCTL_LEDS_OFF:
			iowrite32( 0x0, lp->base_addr + Reg0_LED_1 );
		break;
	}
return 0;
}


static const struct file_operations driver_fops = {
        .owner = THIS_MODULE,
        .unlocked_ioctl = led_control_ioctl,
};


static irqreturn_t pl_led_driver_irq(int irq, void *lp)
{
	printk("pl-led-driver interrupt\n");
	return IRQ_HANDLED;
}

static int pl_led_driver_probe(struct platform_device *pdev)
{
	struct resource *r_irq; /* Interrupt resources */
	struct resource *r_mem; /* IO mem resources */
	struct device *dev = &pdev->dev;
	struct pl_led_driver_local *lp = NULL;

	int rc = 0;
	dev_info(dev, "Device Tree Probing\n");
	/* Get iospace for the device */
	r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r_mem) {
		dev_err(dev, "invalid address\n");
		return -ENODEV;
	}
	lp = (struct pl_led_driver_local *) kmalloc(sizeof(struct pl_led_driver_local), GFP_KERNEL);
	if (!lp) {
		dev_err(dev, "Cound not allocate pl-led-driver device\n");
		return -ENOMEM;
	}
	dev_set_drvdata(dev, lp);
	lp->mem_start = r_mem->start;
	lp->mem_end = r_mem->end;

	if (!request_mem_region(lp->mem_start,
				lp->mem_end - lp->mem_start + 1,
				DRIVER_NAME)) {
		dev_err(dev, "Couldn't lock memory region at %p\n",
			(void *)lp->mem_start);
		rc = -EBUSY;
		goto error1;
	}

	lp->base_addr = ioremap(lp->mem_start, lp->mem_end - lp->mem_start + 1);
	if (!lp->base_addr) {
		dev_err(dev, "pl-led-driver: Could not allocate iomem\n");
		rc = -EIO;
		goto error2;
	}

	/* Get IRQ for the device */
	r_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!r_irq) {
		dev_info(dev, "no IRQ found\n");
		dev_info(dev, "pl-led-driver at 0x%08x mapped to 0x%08x\n",
			(unsigned int __force)lp->mem_start,
			(unsigned int __force)lp->base_addr);
		return 0;
	}
	lp->irq = r_irq->start;
	rc = request_irq(lp->irq, &pl_led_driver_irq, 0, DRIVER_NAME, lp);
	if (rc) {
		dev_err(dev, "testmodule: Could not allocate interrupt %d.\n",
			lp->irq);
		goto error3;
	}

	dev_info(dev,"pl-led-driver at 0x%08x mapped to 0x%08x, irq=%d\n",
		(unsigned int __force)lp->mem_start,
		(unsigned int __force)lp->base_addr,
		lp->irq);
	return 0;

	alloc_chrdev_region(&lp->devid, 0, DEV_CNT, DRIVER_NAME);
    cdev_init(&lp->cdev, &driver_fops);
    cdev_add(&lp->cdev, lp->devid, DEV_CNT);

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


error3:
	free_irq(lp->irq, lp);
error2:
	release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
error1:
	kfree(lp);
	dev_set_drvdata(dev, NULL);
	return rc;
}

static int pl_led_driver_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct pl_led_driver_local *lp = dev_get_drvdata(dev);
	free_irq(lp->irq, lp);
	iounmap(lp->base_addr);
	release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
	kfree(lp);
	dev_set_drvdata(dev, NULL);
	return 0;
}

static struct of_device_id pl_led_driver_of_match[] = {
	{ .compatible = "leds-toggler", },
	{ /* end of list */ },
}; MODULE_DEVICE_TABLE(of, pl_led_driver_of_match);


static struct platform_driver pl_led_driver_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table	= pl_led_driver_of_match,
	},
	.probe		= pl_led_driver_probe,
	.remove		= pl_led_driver_remove,
};

static int __init pl_led_driver_init(void)
{

	return platform_driver_register(&pl_led_driver_driver);
}


static void __exit pl_led_driver_exit(void)
{
	platform_driver_unregister(&pl_led_driver_driver);
	printk(KERN_ALERT "Goodbye module world.\n");
}

module_init(pl_led_driver_init);
module_exit(pl_led_driver_exit);
