#ifndef NXTCOMMS
#define NXTCOMMS

#include <iostream>
#include <usb.h>

static const int NXT_VENDOR_ID = 0x0694;
static const int NXT_PRODUCT_ID = 0x0002;

static usb_dev_handle *pUSBHandle = 0;
static unsigned char ucEpOut = 0;
static unsigned char ucEpIn = 0;
static const int TIMEOUT = 0x0BB8;

static struct usb_device *DevInit(void)
{
	struct usb_bus *usb_bus;
	struct usb_device *dev;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	for (usb_bus = usb_busses; usb_bus; usb_bus = usb_bus->next)
	{
		for (dev = usb_bus->devices; dev; dev = dev->next)
		{
			if ((dev->descriptor.idVendor == NXT_VENDOR_ID) && (dev->descriptor.idProduct == NXT_PRODUCT_ID))
				return dev;
		}
	}
	return 0;
}

static bool DevOpen();

#endif /* NXTCOMMS */
