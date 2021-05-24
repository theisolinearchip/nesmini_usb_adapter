// From https://www.partsnotincluded.com/understanding-the-xbox-360-wired-controllers-usb-data/

PROGMEM const char usbDescriptorDevice[] = {
	0x12,        // bLength
	0x01,        // bDescriptorType
	0x00, 0x02,  // bcdUSB (2.0)
	0xFF,        // bDeviceClass
	0xFF,        // bDeviceSubClass
	0xFF,        // bDeviceProtocol
	0x08,        // bMaxPacketSize0
	0x5E, 0x04,  // idEVendor (Microsoft Corp.)
	0x8E, 0x02,  // idProduct (Xbox360 Controller)
	0x14, 0x01,  // bcdDevice
	0x01,        // iManufacturer
	0x02,        // iProduct
	0x03,        // iSerialNumber
	0x01,        // bNumConfigurations
};

PROGMEM const char usbDescriptorConfiguration[] = {
	// Configuration Descriptor
	0x09,        // bLength
	0x02,        // bDescriptorType (CONFIGURATION)
	0x99, 0x00,  // wTotalLength (153)
	0x04,        // bNumInterfaces
	0x01,        // bConfigurationValue
	0x00,        // iConfiguration
	0xA0,        // bmAttributes
	0xFA,        // bMaxPower
 
	/* ---------------------------------------------------- */
	// Interface 0: Control Data
	0x09,        // bLength
	0x04,        // bDescriptorType (INTERFACE)
	0x00,        // bInterfaceNumber
	0x00,        // bAlternateSetting
	0x02,        // bNumEndpoints
	0xFF,        // bInterfaceClass
	0x5D,        // bInterfaceSubClass
	0x01,        // bInterfaceProtocol
	0x00,        // iInterface
 
	// Unknown Descriptor (If0)
	0x11,        // bLength
	0x21,        // bDescriptorType
	0x00, 0x01, 0x01, 0x25,  // ???
	0x81,        // bEndpointAddress (IN, 1)
	0x14,        // bMaxDataSize
	0x00, 0x00, 0x00, 0x00, 0x13,  // ???
	0x01,        // bEndpointAddress (OUT, 1)
	0x08,        // bMaxDataSize
	0x00, 0x00,  // ???
 
	// Endpoint 1: Control Surface Send
	0x07,        // bLength
	0x05,        // bDescriptorType (ENDPOINT)
	0x81,        // bEndpointAddress (IN, 1)
	0x03,        // bmAttributes
	0x20, 0x00,  // wMaxPacketSize
	0x04,        // bInterval
 
	// Endpoint 1: Control Surface Receive
	0x07,        // bLength
	0x05,        // bDescriptorType (ENDPOINT)
	0x01,        // bEndpointAddress (OUT, 1)
	0x03,        // bmAttributes
	0x20, 0x00,  // wMaxPacketSize
	0x08,        // bInterval
 
	/* ---------------------------------------------------- */
	// Interface 1: Headset (and Expansion Port?)
	0x09,        // bLength
	0x04,        // bDescriptorType (INTERFACE)
	0x01,        // bInterfaceNumber
	0x00,        // bAlternateSetting
	0x04,        // bNumEndpoints
	0xFF,        // bInterfaceClass
	0x5D,        // bInterfaceSubClass
	0x03,        // bInterfaceProtocol
	0x00,        // iInterface
 
	// Unknown Descriptor (If1)
	0x1B,        // bLength
	0x21,        // bDescriptorType
	0x00, 0x01, 0x01, 0x01,  // ???
	0x82,        // bEndpointAddress (IN, 2)
	0x40,        // bMaxDataSize
	0x01,        // ???
	0x02,        // bEndpointAddress (OUT, 2)
	0x20,        // bMaxDataSize
	0x16,        // ???
	0x83,        // bEndpointAddress (IN, 3)
	0x00,        // bMaxDataSize
	0x00, 0x00, 0x00, 0x00, 0x00, 0x16,  // ???
	0x03,        // bEndpointAddress (OUT, 3)
	0x00,        // bMaxDataSize
	0x00, 0x00, 0x00, 0x00, 0x00,  // ???
 
	// Endpoint 2: Microphone Data Send
	0x07,        // bLength
	0x05,        // bDescriptorType (ENDPOINT)
	0x82,        // bEndpointAddress (IN, 2)
	0x03,        // bmAttributes
	0x20, 0x00,  // wMaxPacketSize
	0x02,        // bInterval
 
	// Endpoint 2: Headset Audio Receive
	0x07,        // bLength
	0x05,        // bDescriptorType (ENDPOINT)
	0x02,        // bEndpointAddress (OUT, 2)
	0x03,        // bmAttributes
	0x20, 0x00,  // wMaxPacketSize
	0x04,        // bInterval
 
	// Endpoint 3: Unknown, Send
	0x07,        // bLength
	0x05,        // bDescriptorType (ENDPOINT)
	0x83,        // bEndpointAddress (IN, 3)
	0x03,        // bmAttributes
	0x20, 0x00,  // wMaxPacketSize
	0x40,        // bInterval
 
	// Endpoint 3: Unknown, Receive
	0x07,        // bLength
	0x05,        // bDescriptorType (ENDPOINT)
	0x03,        // bEndpointAddress (OUT, 3)
	0x03,        // bmAttributes
	0x20, 0x00,  // wMaxPacketSize
	0x10,        // bInterval
 
	/* ---------------------------------------------------- */
	// Interface 2: Unknown
	0x09,        // bLength
	0x04,        // bDescriptorType (INTERFACE)
	0x02,        // bInterfaceNumber
	0x00,        // bAlternateSetting
	0x01,        // bNumEndpoints
	0xFF,        // bInterfaceClass
	0x5D,        // bInterfaceSubClass
	0x02,        // bInterfaceProtocol
	0x00,        // iInterface
 
	// Unknown Descriptor (If2)
	0x09,        // bLength
	0x21,        // bDescriptorType
	0x00, 0x01, 0x01, 0x22,  // ???
	0x84,        // bEndpointAddress (IN, 4)
	0x07,        // bMaxDataSize
	0x00,        // ???
 
	// Endpoint 4: Unknown, Send
	0x07,        // bLength
	0x05,        // bDescriptorType (ENDPOINT)
	0x84,        // bEndpointAddress (IN, 4)
	0x03,        // bmAttributes
	0x20, 0x00,  // wMaxPacketSize
	0x10,        // bInterval
 
	/* ---------------------------------------------------- */
	// Interface 3: Security Method
	0x09,        // bLength
	0x04,        // bDescriptorType (INTERFACE)
	0x03,        // bInterfaceNumber
	0x00,        // bAlternateSetting
	0x00,        // bNumEndpoints
	0xFF,        // bInterfaceClass
	0xFD,        // bInterfaceSubClass
	0x13,        // bInterfaceProtocol
	0x04,        // iInterface
 
	// Unknown Descriptor (If3)
	0x06,        // bLength
	0x41,        // bDescriptorType
	0x00, 0x01, 0x01, 0x03,  // ???
};