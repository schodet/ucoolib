/* ucoolib - Microcontroller object oriented library. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * }}} */
#include "usb_desc.stm32.h"

#include "config/ucoo/hal/usb.hh"

const struct usb_device_descriptor usb_desc_dev = {
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0xff,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 64,
    .idVendor = CONFIG_UCOO_HAL_USB_VENDOR_ID,
    .idProduct = CONFIG_UCOO_HAL_USB_PRODUCT_ID,
    .bcdDevice = 0x0000,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 0,
    .bNumConfigurations = 1,
};

static const struct usb_endpoint_descriptor usb_desc_endp_1[] = {
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x01,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = CONFIG_UCOO_HAL_USB_EP_SIZE,
	.bInterval = 0,
    },
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x81,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = CONFIG_UCOO_HAL_USB_EP_SIZE,
	.bInterval = 0,
    },
};

static const struct usb_interface_descriptor usb_desc_iface_1[] = {
    {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 2,
	.bInterfaceClass = 0xff,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = usb_desc_endp_1,
    },
};

#if CONFIG_UCOO_HAL_USB_STREAM_NB >= 2

static const struct usb_endpoint_descriptor usb_desc_endp_2[] = {
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x02,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = CONFIG_UCOO_HAL_USB_EP_SIZE,
	.bInterval = 0,
    },
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x82,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = CONFIG_UCOO_HAL_USB_EP_SIZE,
	.bInterval = 0,
    },
};

static const struct usb_interface_descriptor usb_desc_iface_2[] = {
    {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 1,
	.bAlternateSetting = 0,
	.bNumEndpoints = 2,
	.bInterfaceClass = 0xff,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = usb_desc_endp_2,
    },
};

#endif /* CONFIG_UCOO_HAL_USB_STREAM_NB >= 2 */

#if CONFIG_UCOO_HAL_USB_STREAM_NB >= 3

static const struct usb_endpoint_descriptor usb_desc_endp_3[] = {
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x03,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = CONFIG_UCOO_HAL_USB_EP_SIZE,
	.bInterval = 0,
    },
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x83,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = CONFIG_UCOO_HAL_USB_EP_SIZE,
	.bInterval = 0,
    },
};

static const struct usb_interface_descriptor usb_desc_iface_3[] = {
    {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 2,
	.bAlternateSetting = 0,
	.bNumEndpoints = 2,
	.bInterfaceClass = 0xff,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = usb_desc_endp_3,
    },
};

#endif /* CONFIG_UCOO_HAL_USB_STREAM_NB >= 3 */

#if CONFIG_UCOO_HAL_USB_STREAM_NB > 3
# error "too many streams requested"
#endif

static const struct usb_interface usb_desc_ifaces[] = {
    {
	.num_altsetting = 1,
	.altsetting = usb_desc_iface_1,
    },
#if CONFIG_UCOO_HAL_USB_STREAM_NB >= 2
    {
	.num_altsetting = 1,
	.altsetting = usb_desc_iface_2,
    },
#endif
#if CONFIG_UCOO_HAL_USB_STREAM_NB >= 3
    {
	.num_altsetting = 1,
	.altsetting = usb_desc_iface_3,
    },
#endif
};

const struct usb_config_descriptor usb_desc_config = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType = USB_DT_CONFIGURATION,
    .wTotalLength = 0,
    .bNumInterfaces = CONFIG_UCOO_HAL_USB_STREAM_NB,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80 | (CONFIG_UCOO_HAL_USB_SELF_POWERED ? 0x40 : 0),
    .bMaxPower = CONFIG_UCOO_HAL_USB_MAX_POWER / 2,

    .interface = usb_desc_ifaces,
};

