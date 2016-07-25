#ifndef ucoo_hal_usb_usb_cdc_def_hh
#define ucoo_hal_usb_usb_cdc_def_hh
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
#include "ucoo/hal/usb/usb_def.hh"

namespace ucoo {

enum
{
    USB_CDC_NOTIFICATION_SERIAL_STATE = 0x20,
};

/// Agglomeration of bmRequestType and bRequest.
enum
{
    USB_CDC_REQ_SET_CONTROL_LINE_STATE = USB_REQ (INTERFACE, CLASS, OUT, 0x22),
    USB_CDC_REQ_SET_LINE_CODING = USB_REQ (INTERFACE, CLASS, OUT, 0x20),
};

/// Line coding structure.
struct __attribute__ ((packed, aligned (1))) UsbCdcLineCoding
{
    /// Data terminal rate in bits per second.
    uint32_t dwDTERate;
    /// Stop bits:
    ///  - 0: 1 stop bit
    ///  - 1: 1.5 stop bits
    ///  - 2: 2 stop bits
    uint8_t bCharFormat;
    /// Parity:
    ///  - 0: None
    ///  - 1: Odd
    ///  - 2: Even
    ///  - 3: Mark
    ///  - 4: Space
    uint8_t bParityType;
    /// Data bits, 5, 6, 7, 8 or 16.
    uint8_t bDataBits;
};

/// Notification structure.
struct __attribute__ ((packed, aligned (1))) UsbCdcNotification
{
    uint8_t bmRequestType;
    uint8_t bNotification;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
};

} // namespace ucoo

#endif // ucoo_hal_usb_usb_cdc_def_hh
