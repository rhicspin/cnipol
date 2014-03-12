/*
 * Igor Alekseev & Dmitry Svirida @itep.ru, 2005
 * USB CMC100 CAMAC crate controller driver made from 
 *
 * USB Skeleton driver - 2.0
 *
 * Copyright (C) 2001-2004 Greg Kroah-Hartman (greg@kroah.com)
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License as
 *	published by the Free Software Foundation, version 2.
 *
 * This driver is based on the 2.6.3 version of drivers/usb/usb-skeleton.c 
 * but has been rewritten to be easy to read and use, as no locks are now
 * needed anymore.
 *
 */
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/smp_lock.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <asm/uaccess.h>
#include <linux/usb.h>
#include <linux/ioctl.h>

#define CMCSLEEP	5000		// 5 sec.
#define CMCTMO		200		// 0.2 sec
#define CMCBLMPLY	8
/*	Our endpoints		*/	
#define CMC_CHAIN_OUT	0x02
#define CMC_DATA_IN	0x86
#define CMC_CONTROL_OUT	0x01
#define CMC_CONTROL_IN	0x81

/* Version Information */
#define DRIVER_VERSION "v0.3"
#define DRIVER_AUTHOR "Igor Alekseev & Dmitry Svirida @itep.ru"
#define DRIVER_DESC "USB CMC100 CAMAC Crate Controller Driver"

/* Define these values to match your devices */
#define USB_CMCAMAC_VENDOR_ID	0x1279
#define USB_CMCAMAC_PRODUCT_ID	0x64

#define CMC_CTL_RESET	_IO('Z', 0)
#define CMC_CTL_R1	_IO('Z', 1)

#ifndef BUS_ID_SIZE
#define BUS_ID_SIZE	32
#endif

#ifndef info
#define info(format, arg...)					\
	printk(KERN_INFO KBUILD_MODNAME ": " format "\n", ##arg)
#endif

/* table of devices that work with this driver */
static struct usb_device_id cmcamac_table [] = {
	{ USB_DEVICE(USB_CMCAMAC_VENDOR_ID, USB_CMCAMAC_PRODUCT_ID) },
	{ }					/* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, cmcamac_table);


/* Get a minor range for your devices from the usb maintainer */
#define USB_CMCAMAC_MINOR_BASE	192
#define CMCAMAC_MAX_MINORS	8

typedef enum {CMC_NONE = 0, CMC_READY, CMC_BUSY} CMC_STATE;
static CMC_STATE cmcamac_minor_table[CMCAMAC_MAX_MINORS];	/* table of occupied minors	*/

/* Structure to hold all of our device specific stuff */
struct usb_cmcamac {
	struct usb_device *	udev;			/* the usb device for this device */
	struct usb_interface *	interface;		/* the interface for this device */
	unsigned char *		bulk_in_buffer;		/* the buffer to receive data */
	size_t			bulk_in_size;		/* the size of the receive buffer */
	__u8			bulk_in_endpointAddr;	/* the address of the bulk in endpoint */
	unsigned char *		bulk_out_buffer;	/* the buffer to receive data */
	size_t			bulk_out_size;		/* the size of the receive buffer */
	__u8			bulk_out_endpointAddr;	/* the address of the bulk out endpoint */
	size_t			control_out_size;	/* bulk endpoint for reset */
	__u8			control_out_endpointAddr;
	size_t			control_in_size;	/* bulk endpoint for debug readout */
	__u8			control_in_endpointAddr;
	struct kref		kref;
};
#define to_cmcamac_dev(d) container_of(d, struct usb_cmcamac, kref)

static struct usb_driver cmcamac_driver;

/*
    Internal send
*/
static ssize_t cmcamac_send(struct usb_cmcamac *dev, void *buf, size_t count)
{
	int retval = 0;
	int cnt;

	/* verify that we actually have some data to write */
	if (count == 0)	return 0;
	memcpy(dev->bulk_out_buffer, buf, min(dev->bulk_out_size, count));

	/* do a blocking bulk write to put data to the device */
	retval = usb_bulk_msg(dev->udev,
			      usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
			      dev->bulk_out_buffer,
			      min(dev->bulk_out_size, count),
			      &cnt, CMCTMO);
	if (retval) {
		err("%s - failed writing, error %d", __FUNCTION__, retval);
		return retval;
	}

	return cnt;
}

/*
    Internal receive
*/
static ssize_t cmcamac_receive(struct usb_cmcamac *dev, void *buf, size_t count)
{
	int retval = 0;
	int cnt;
	
	/* do a blocking bulk read to get data from the device */
	retval = usb_bulk_msg(dev->udev,
			      usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
			      dev->bulk_in_buffer,
			      min(dev->bulk_in_size, count),
			      &cnt, CMCTMO);

	/* if the read was successful, copy the data to userspace */
	if (!retval) {
		memcpy(buf, dev->bulk_in_buffer, cnt);
		retval = cnt;
	}

	return retval;
}

/*
    Empty controller buffer, get controller number and return it
*/
#define MAXREADS	10000	// ~ 1048000 CAMAC words / 512 byte USB block

static int cmcamac_getnumber(struct usb_cmcamac *dev)
{
	int sndbuf[] = {0xFFFFFFFF, 0, 0x3C11, 0x0E000000};
	int buf[128];
	int retval = 0;
	int i;
	int ecnt;
	int reseted = 0;
AGAIN:
/*		Send something with PKTEND		*/	
	retval = cmcamac_send(dev, &sndbuf, sizeof(sndbuf));
	if (retval < 0) return retval;
	if (retval != sizeof(sndbuf)) {
		err("Unexpected error writing to controller.");
		retval = -ECOMM;
		goto RESET;
	}
/*		Send something with PKTEND		*/	
	retval = cmcamac_send(dev, &sndbuf, sizeof(sndbuf));
	if (retval < 0) return retval;
	if (retval != sizeof(sndbuf)) {
		err("Unexpected error writing to controller.");
		retval = -ECOMM;
		goto RESET;
	}
/*		Try to read until timeout - this is the 
		only method to clear data FIFO		*/
	for (i=0, ecnt=0; i<MAXREADS; i++) {
		retval = cmcamac_receive(dev, &buf, sizeof(buf));
		if (retval == -ETIMEDOUT) break;
		if (retval < 0) {
			ecnt++;
			if (ecnt > 5) {
				err("Read error %d.", retval);
				goto RESET;
			}
		}	
	}
	if (i == MAXREADS) {
		err("Too many reads required to empty controller.");
		retval = -ECOMM;
		goto RESET;
	}
/*		Send N30 F1 A1 - read hardware version	*/
	retval = cmcamac_send(dev, &sndbuf, sizeof(sndbuf));
	if (retval < 0) return retval;
	if (retval != sizeof(sndbuf)) {
		err("Unexpected error writing to controller.");
		retval = -ECOMM;
		goto RESET;
	}
/*		Read data to get crate number		*/
	retval = cmcamac_receive(dev, &buf, sizeof(buf));
	if (retval < 0) return retval;
	if (retval != sizeof(int)) {
		err("Unexpected error reading from controller. Retval = %d.", retval);
		retval = -ECOMM;
		goto RESET;
	}
	
	retval = (buf[0] >> 28)	& 7;
	return retval;
/*		Do hard reset				*/
RESET:
	if (reseted > 5) {
	    err("Fatal too many attempts to reset controller.");
	    return retval;
	}
	info("Unable to read crate number -> doing hard reset.");
	if (dev->control_out_endpointAddr != 0) {
		retval = usb_bulk_msg(dev->udev,
			usb_sndbulkpipe(dev->udev, dev->control_out_endpointAddr),
			&sndbuf[1], 1, &i, CMCTMO);
		if (retval < 0) return retval;
	}	
	msleep(CMCSLEEP*2);
	reseted++;
	goto AGAIN;
}

static void cmcamac_delete(struct kref *kref)
{	
	struct usb_cmcamac *dev = to_cmcamac_dev(kref);

	usb_put_dev(dev->udev);
	kfree (dev->bulk_in_buffer);
	kfree (dev->bulk_out_buffer);
	kfree (dev);
}

static int cmcamac_open(struct inode *inode, struct file *file)
{
	struct usb_cmcamac *dev;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;
	int i;

	subminor = iminor(inode);

	lock_kernel();
	switch(cmcamac_minor_table[subminor - USB_CMCAMAC_MINOR_BASE]) {
	case CMC_NONE:
		unlock_kernel();
		retval = -ENODEV;
		break;
	case CMC_BUSY:
		unlock_kernel();
		err("Only one process can use CMCAMAC crate #%d.", subminor - USB_CMCAMAC_MINOR_BASE);
		retval = -EBUSY;
		break;
	case CMC_READY:
		cmcamac_minor_table[subminor - USB_CMCAMAC_MINOR_BASE] = CMC_BUSY;
		unlock_kernel();
		break;
	}
	if (retval != 0) goto exit;
	
	interface = usb_find_interface(&cmcamac_driver, subminor);
	if (!interface) {
		err ("%s - error, can't find device for minor %d",
		     __FUNCTION__, subminor);
		cmcamac_minor_table[subminor - USB_CMCAMAC_MINOR_BASE] = CMC_NONE;
		retval = -ENODEV;
		goto exit;
	}

	dev = usb_get_intfdata(interface);
	if (!dev) {
		cmcamac_minor_table[subminor - USB_CMCAMAC_MINOR_BASE] = CMC_NONE;
		retval = -ENODEV;
		goto exit;
	}
	/* Check if this is really a controller one asks for and flush buffers */
	i = cmcamac_getnumber(dev);
	if (i < 0) {
		err("Unable to read crate number. May be crate is offline.");
		cmcamac_minor_table[subminor - USB_CMCAMAC_MINOR_BASE] = CMC_NONE;
		retval = i;
		goto exit;
	}
	if (i + USB_CMCAMAC_MINOR_BASE != subminor) {
		err("Crate number unexpectedly changed from %d to %d", subminor - USB_CMCAMAC_MINOR_BASE, i);
		cmcamac_minor_table[subminor - USB_CMCAMAC_MINOR_BASE] = CMC_NONE;
		retval = -ENODEV;
		goto exit;
	}

	/* increment our usage count for the device */
	kref_get(&dev->kref);

	/* save our object in the file's private structure */
	file->private_data = dev;

exit:
	return retval;
}

static int cmcamac_release(struct inode *inode, struct file *file)
{
	struct usb_cmcamac *dev;
	int subminor;

	dev = (struct usb_cmcamac *)file->private_data;
	if (dev == NULL)
		return -ENODEV;

	subminor = iminor(inode);
	lock_kernel();
	if (cmcamac_minor_table[subminor - USB_CMCAMAC_MINOR_BASE] == CMC_BUSY)
	    cmcamac_minor_table[subminor - USB_CMCAMAC_MINOR_BASE] = CMC_READY;
	unlock_kernel();
	/* decrement the count on our device */
	kref_put(&dev->kref, cmcamac_delete);
	return 0;
}

static ssize_t cmcamac_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
	struct usb_cmcamac *dev;
	int retval = 0;
	size_t rcnt;
	int l;
	int i;

	if (count <= 0) return 0;

	dev = (struct usb_cmcamac *)file->private_data;
	
	rcnt = 0;
	for (i=0;i<MAXREADS;i++) {
		/* do a blocking bulk read to get data from the device */
		retval = usb_bulk_msg(dev->udev,
			      usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
			      dev->bulk_in_buffer,
			      dev->bulk_in_size,
			      &l, CMCTMO);
		if (retval) {
			err("CMCAMAC read error %d.", retval);
			return retval;
		}

		/* Copy the data to userspace */
		if (count > rcnt && l > 0 &&  
			copy_to_user(&buffer[rcnt], dev->bulk_in_buffer, min((size_t)l, count-rcnt))) 
				return -EFAULT;
		rcnt += l;
		if (l != dev->bulk_in_size) return rcnt;
	}
	err("Too many reads required to empty controller.");
	return -EFAULT;	
}

static ssize_t cmcamac_write(struct file *file, const char *user_buffer, size_t count, loff_t *ppos)
{
	struct usb_cmcamac *dev;
	int retval = 0;
	size_t wcnt;
	int l;

	if (count <= 0) return 0;

	dev = (struct usb_cmcamac *)file->private_data;
	/* verify that we actually have some data to write */

	for (wcnt = 0; wcnt < count; wcnt += l) {
		l = min(dev->bulk_out_size, count-wcnt);
		if (copy_from_user(dev->bulk_out_buffer, &user_buffer[wcnt], l)) {
			retval = -EFAULT;
			return retval;
		}

	/* do a blocking bulk write to put data to the device */
		retval = usb_bulk_msg(dev->udev,
			      usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
			      dev->bulk_out_buffer,
			      l, &l, CMCTMO);
		if (retval) {
			err("%s - failed writing, error %d", __FUNCTION__, retval);
			return retval;
		}
	}
	return wcnt;

}

static int cmcamac_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct usb_cmcamac *dev;
	int cnt;
	int data = 0;
	int retval = -ENOIOCTLCMD;

	dev = (struct usb_cmcamac *)file->private_data;
	switch (cmd) {
	case CMC_CTL_RESET :
		if (dev->control_out_endpointAddr == 0) break;
		retval = usb_bulk_msg(dev->udev,
			usb_sndbulkpipe(dev->udev, dev->control_out_endpointAddr),
			&data, 1, &cnt, CMCTMO);
		break;
	case CMC_CTL_R1 :
		if (dev->control_in_endpointAddr == 0) break;
		retval = usb_bulk_msg(dev->udev,
			usb_rcvbulkpipe(dev->udev, dev->control_in_endpointAddr),
			&data, 1, &cnt, CMCTMO);
		if (retval < 0) break;
		retval = data & 0xFF;
		break;
	}
	return retval;
}

static struct file_operations cmcamac_fops = {
	.owner =	THIS_MODULE,
	.read =		cmcamac_read,
	.write =	cmcamac_write,
	.open =		cmcamac_open,
	.release =	cmcamac_release,
	.ioctl = 	cmcamac_ioctl,
};

/* 
 * usb class driver info in order to get a minor number from the usb core,
 * and to have the device registered with devfs and the driver core
 */
static struct usb_class_driver cmcamac_class = {
	.name =		NULL,
	.fops =		&cmcamac_fops,
//	.mode =		S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
	.minor_base =	-1,
};

static int cmcamac_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_cmcamac *dev = NULL;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	size_t buffer_size;
	int i;
	int retval = -ENOMEM;
	char name[BUS_ID_SIZE];

	/* allocate memory for our device state and initialize it */
	dev = kmalloc(sizeof(*dev), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		goto error;
	}
	memset(dev, 0x00, sizeof(*dev));
	kref_init(&dev->kref);

	dev->udev = usb_get_dev(interface_to_usbdev(interface));
	dev->interface = interface;

	/* set up the endpoint information */
	/* use only the first bulk-in and bulk-out endpoints */
	iface_desc = interface->cur_altsetting;
	for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
		endpoint = &iface_desc->endpoint[i].desc;

/*	DATA input endpoint		*/
		if ((endpoint->bEndpointAddress == CMC_DATA_IN) &&
			((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_BULK)) {
			/* we found a bulk in endpoint */
			buffer_size = CMCBLMPLY * endpoint->wMaxPacketSize;
			dev->bulk_in_size = buffer_size;
			dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
			dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
			if (!dev->bulk_in_buffer) {
		    		err("Could not allocate bulk_in_buffer");
				goto error;
			}
		}
/*	CHAIN output endpoint		*/
		if ((endpoint->bEndpointAddress == CMC_CHAIN_OUT) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_BULK)) {
			/* we found a bulk out endpoint */
			buffer_size = CMCBLMPLY * endpoint->wMaxPacketSize;
			dev->bulk_out_size = buffer_size;
			dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
			dev->bulk_out_buffer = kmalloc(buffer_size, GFP_KERNEL);
			if (!dev->bulk_out_buffer) {
				err("Could not allocate bulk_out_buffer");
				goto error;
			}
		}
/*	CONTROL input endpoint		*/
		if ((endpoint->bEndpointAddress == CMC_CONTROL_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_BULK)) {
			/* we found a control in endpoint */
			dev->control_in_size = endpoint->wMaxPacketSize;
			dev->control_in_endpointAddr = endpoint->bEndpointAddress;
		}
/*	CONTROL output endpoint		*/
		if ((endpoint->bEndpointAddress == CMC_CONTROL_OUT) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_BULK)) {
			/* we found a control out endpoint */
			dev->control_out_size = endpoint->wMaxPacketSize;
			dev->control_out_endpointAddr = endpoint->bEndpointAddress;
		}
		
	}
	if (!(dev->bulk_in_endpointAddr && dev->bulk_out_endpointAddr)) {
		err("Could not find both bulk-in and bulk-out endpoints");
		goto error;
	}

/*		Sleep here, but not at open. Don't know why we need this.	*/
	msleep(CMCSLEEP);
	i = cmcamac_getnumber(dev);
	if (i < 0) {	// Let's try again. Otherwise we always fail at power on.
	    msleep(10*CMCSLEEP);
	    i = cmcamac_getnumber(dev);	// Let's try again. Otherwise we always fail at power on.
	}
	if (i < 0) {
		err("Unable to read crate number. May be crate is offline.");
		retval = i;
		goto error;
	}

	if (cmcamac_minor_table[i] != CMC_NONE) {
		err("We already have controller with this number (%d).", i);
		retval = -EINVAL;
		goto error;
	}

	/* save our data pointer in this interface device */
	usb_set_intfdata(interface, dev);

	cmcamac_class.minor_base = i + USB_CMCAMAC_MINOR_BASE;
	snprintf(name, sizeof(name), "usb/cmcamac%d", i);
	cmcamac_class.name = name;

	/* we can register the device now, as it is ready */
	retval = usb_register_dev(interface, &cmcamac_class);
	if (retval) {
		/* something prevented us from registering this driver */
		err("Not able to get a minor for this device.");
		usb_set_intfdata(interface, NULL);
		goto error;
	}

	cmcamac_minor_table[i] = CMC_READY;
	
	/* let the user know what node this device is now attached to */
	info("USB CMCAMAC #%d attached to minor %d", i, interface->minor);
	return 0;

error:
	if (dev)
		kref_put(&dev->kref, cmcamac_delete);
	return retval;
}

static void cmcamac_disconnect(struct usb_interface *interface)
{
	char name[BUS_ID_SIZE];
	struct usb_cmcamac *dev;
	int minor = interface->minor;

	/* prevent cmcamac_open() from racing cmcamac_disconnect() */
	lock_kernel();

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	/* give back our minor */
	cmcamac_class.minor_base = minor;
	snprintf(name, sizeof(name), "usb/cmcamac%d", minor - USB_CMCAMAC_MINOR_BASE);
	cmcamac_class.name = name;
	usb_deregister_dev(interface, &cmcamac_class);
	cmcamac_minor_table[minor - USB_CMCAMAC_MINOR_BASE] = CMC_NONE;
	
	unlock_kernel();

	/* decrement our usage count */
	kref_put(&dev->kref, cmcamac_delete);

	info("USB CMCAMAC #%d now disconnected", minor - USB_CMCAMAC_MINOR_BASE);
}

static struct usb_driver cmcamac_driver = {
//	.owner =	THIS_MODULE,
	.name =		"cmcamac",
	.probe =	cmcamac_probe,
	.disconnect =	cmcamac_disconnect,
	.id_table =	cmcamac_table,
};

static int __init usb_cmcamac_init(void)
{
	int result;

	memset(cmcamac_minor_table, 0, sizeof(cmcamac_minor_table));
	/* register this driver with the USB subsystem */
	result = usb_register(&cmcamac_driver);
	if (result)
		err("usb_register failed. Error number %d", result);

	return result;
}

static void __exit usb_cmcamac_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&cmcamac_driver);
}

module_init (usb_cmcamac_init);
module_exit (usb_cmcamac_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRIVER_VERSION);
