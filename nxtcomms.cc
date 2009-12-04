#include "nxtcomms.h"
#include <iostream>


static bool DevOpen()
{
	struct usb_device *usb_dev;
	int nEp = 0;

	usb_dev = DevInit();

	if (usb_dev == 0)
	{
		std::cerr << "Device not found!" << std::endl;
		return 0;
	}

	pUSBHandle = usb_open(usb_dev);

	if (pUSBHandle == 0)
	{
		std::cerr << "Not able to claim the USB device" << std::endl;
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
