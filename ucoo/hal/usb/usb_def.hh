#ifndef ucoo_hal_usb_usb_def_hh
#define ucoo_hal_usb_usb_def_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2016 Nicolas Schodet
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// }}}

namespace ucoo {

enum
{
    /// Only support one control end point size.
    USB_CONTROL_END_POINT_SIZE = 64,
};

/// Agglomeration of bmRequestType and bRequest.
enum
{
    USB_REQ_TYPE_DEVICE = 0x0000,
    USB_REQ_TYPE_INTERFACE = 0x0001,
    USB_REQ_TYPE_END_POINT = 0x0002,
    USB_REQ_TYPE_OTHER = 0x0003,
    USB_REQ_TYPE_STANDARD = 0x0000,
    USB_REQ_TYPE_CLASS = 0x0020,
    USB_REQ_TYPE_VENDOR = 0x0040,
    USB_REQ_TYPE_OUT = 0x0000,
    USB_REQ_TYPE_IN = 0x0080,
#define USB_REQ(rec, type, dir, code) \
    (USB_REQ_TYPE_ ## rec | USB_REQ_TYPE_ ## type | USB_REQ_TYPE_ ## dir \
     | ((code) << 8))
    USB_REQ_CLEAR_FEATURE_DEVICE = USB_REQ (DEVICE, STANDARD, OUT, 0x01),
    USB_REQ_CLEAR_FEATURE_INTERFACE = USB_REQ (INTERFACE, STANDARD, OUT, 0x01),
    USB_REQ_CLEAR_FEATURE_END_POINT = USB_REQ (END_POINT, STANDARD, OUT, 0x01),
    USB_REQ_GET_CONFIGURATION = USB_REQ (DEVICE, STANDARD, IN, 0x08),
    USB_REQ_GET_DESCRIPTOR = USB_REQ (DEVICE, STANDARD, IN, 0x06),
    USB_REQ_GET_INTERFACE = USB_REQ (INTERFACE, STANDARD, IN, 0x0a),
    USB_REQ_GET_STATUS_DEVICE = USB_REQ (DEVICE, STANDARD, IN, 0x00),
    USB_REQ_GET_STATUS_INTERFACE = USB_REQ (INTERFACE, STANDARD, IN, 0x00),
    USB_REQ_GET_STATUS_END_POINT = USB_REQ (END_POINT, STANDARD, IN, 0x00),
    USB_REQ_SET_ADDRESS = USB_REQ (DEVICE, STANDARD, OUT, 0x05),
    USB_REQ_SET_CONFIGURATION = USB_REQ (DEVICE, STANDARD, OUT, 0x09),
    USB_REQ_SET_DESCRIPTOR = USB_REQ (DEVICE, STANDARD, OUT, 0x07),
    USB_REQ_SET_FEATURE_DEVICE = USB_REQ (DEVICE, STANDARD, OUT, 0x03),
    USB_REQ_SET_FEATURE_INTERFACE = USB_REQ (INTERFACE, STANDARD, OUT, 0x03),
    USB_REQ_SET_FEATURE_END_POINT = USB_REQ (END_POINT, STANDARD, OUT, 0x03),
    USB_REQ_SET_INTERFACE = USB_REQ (INTERFACE, STANDARD, OUT, 0x0b),
    USB_REQ_SYNCH_FRAME = USB_REQ (END_POINT, STANDARD, IN, 0x0c),
};

} // namespace ucoo

#endif // ucoo_hal_usb_usb_def_hh
