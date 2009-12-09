//#include <iostream>
#include <usb.h>

#include <unistd.h>
#include <string.h>

#include <libplayercore/playercore.h>



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



static bool DevOpen()
{
	struct usb_device *usb_dev;
	int nEp = 0;

	usb_dev = DevInit();

	if (usb_dev == 0)
	{
		//std::cerr << "Device not found!" << std::endl;
		puts("Device not found!");
		return 0;
	}

	pUSBHandle = usb_open(usb_dev);

	if (pUSBHandle == 0)
	{
		//std::cerr << "Not able to claim the USB device" << std::endl;
		puts("Not able to claim the USB device");
		return 0;
	}
	else
	{
		if (usb_dev->config)
		{
			if (usb_dev->config->interface)
			{
				if (usb_dev->config->interface->altsetting)
				{
					usb_claim_interface(pUSBHandle, usb_dev->config->interface->altsetting->bInterfaceNumber);
					if ((nEp = usb_dev->config->interface->altsetting->bNumEndpoints))
					{
						if (usb_dev->config->interface->altsetting->endpoint)
						{
							ucEpIn = (unsigned char) (usb_dev->config->interface->altsetting->endpoint[0].bEndpointAddress);
							if (nEp > 1)
								ucEpOut = (unsigned char) (usb_dev->config->interface->altsetting->endpoint[1].bEndpointAddress);

						}
					}
				}
			}
		}
	}
	return 1;
}


static void nxt_set_motor(int speed)
{
	unsigned char directCommandBuffer[256];
	if(speed > 0){
		directCommandBuffer[0] = 0x80;
		directCommandBuffer[1] = 0x04;
		directCommandBuffer[2] = 0xFF;
		directCommandBuffer[3] = 0x42;
		directCommandBuffer[4] = 0x01;
		directCommandBuffer[5] = 0x00;
		directCommandBuffer[6] = 0x00;
		directCommandBuffer[7] = 0x20;
		directCommandBuffer[8] = 0x00;
		directCommandBuffer[9] = 0x00;
		directCommandBuffer[10] = 0x00;
		directCommandBuffer[11] = 0x00;
	}else if (speed == 0){
		directCommandBuffer[0] = 0x80;
		directCommandBuffer[1] = 0x04;
		directCommandBuffer[2] = 0xFF;
		directCommandBuffer[3] = 0x00;
		directCommandBuffer[4] = 0x01;
		directCommandBuffer[5] = 0x00;
		directCommandBuffer[6] = 0x00;
		directCommandBuffer[7] = 0x20;
		directCommandBuffer[8] = 0x00;
		directCommandBuffer[9] = 0x00;
		directCommandBuffer[10] = 0x00;
		directCommandBuffer[11] = 0x00;
	}
	usb_bulk_write(pUSBHandle, ucEpIn, reinterpret_cast < char *>(directCommandBuffer), 11, 0x0BB8);
	return;
}

