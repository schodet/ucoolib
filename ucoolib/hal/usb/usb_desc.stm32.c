/* ucoolib - Microcontroller object oriented library. {{{
 *
 * Copyright (C) 2012 Nicolas Schodet
 *
 * APBTeam:
 *        Web: http://apbteam.org/
 *      Email: team AT apbteam DOT org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * }}} */
#include "usb_desc.stm32.h"

#include "config/hal/usb.hh"

const struct usb_device_descriptor usb_desc_dev = {
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0xff,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 64,
    .idVendor = UCOO_CONFIG_HAL_USB_VENDOR_ID,
    .idProduct = UCOO_CONFIG_HAL_USB_PRODUCT_ID,
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
	.wMaxPacketSize = UCOO_CONFIG_HAL_USB_EP_SIZE,
	.bInterval = 0,
    },
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x81,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = UCOO_CONFIG_HAL_USB_EP_SIZE,
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

#if UCOO_CONFIG_HAL_USB_STREAM_NB >= 2

static const struct usb_endpoint_descriptor usb_desc_endp_2[] = {
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x02,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = UCOO_CONFIG_HAL_USB_EP_SIZE,
	.bInterval = 0,
    },
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x82,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = UCOO_CONFIG_HAL_USB_EP_SIZE,
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

#endif /* UCOO_CONFIG_HAL_USB_STREAM_NB >= 2 */

#if UCOO_CONFIG_HAL_USB_STREAM_NB >= 3

static const struct usb_endpoint_descriptor usb_desc_endp_3[] = {
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x03,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = UCOO_CONFIG_HAL_USB_EP_SIZE,
	.bInterval = 0,
    },
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x83,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = UCOO_CONFIG_HAL_USB_EP_SIZE,
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

#endif /* UCOO_CONFIG_HAL_USB_STREAM_NB >= 3 */

#if UCOO_CONFIG_HAL_USB_STREAM_NB > 3
# error "too many streams requested"
#endif

static const struct usb_interface usb_desc_ifaces[] = {
    {
	.num_altsetting = 1,
	.altsetting = usb_desc_iface_1,
    },
#if UCOO_CONFIG_HAL_USB_STREAM_NB >= 2
    {
	.num_altsetting = 1,
	.altsetting = usb_desc_iface_2,
    },
#endif
#if UCOO_CONFIG_HAL_USB_STREAM_NB >= 3
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
    .bNumInterfaces = UCOO_CONFIG_HAL_USB_STREAM_NB,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80 | (UCOO_CONFIG_HAL_USB_SELF_POWERED ? 0x40 : 0),
    .bMaxPower = UCOO_CONFIG_HAL_USB_MAX_POWER / 2,

    .interface = usb_desc_ifaces,
};

