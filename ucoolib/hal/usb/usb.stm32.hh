#ifndef ucoolib_hal_usb_usb_stm32_hh
#define ucoolib_hal_usb_usb_stm32_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
#include "ucoolib/intf/stream.hh"
#include "ucoolib/common.hh"

#include "config/hal/usb.hh"

#include <libopencm3/usb/usbd.h>

namespace ucoo {

class UsbStream;

/// Top control class for an USB device.  This is a limited implementation
/// which fulfill most of our needs: one or more interfaces, each of them is
/// handled as a character stream.  This should be instantiated only once.
class UsbStreamControl
{
  public:
    /// Construct a new USB control object, with given VENDOR and PRODUCT
    /// strings.
    UsbStreamControl (const char *vendor, const char *product);
    /// Return true if the device is configured.  This means that the
    /// connection is done with the host and data can be exchanged.
    bool is_configured () const { return configured_; }
  private:
    /// Called by USB stack when device is configured.  This is expected to
    /// setup endpoints.
    static void set_config (usbd_device *usbdev, uint16_t wValue);
    /// Called by USB stack when a frame is received on a OUT endpoint.
    static void rx_callback (usbd_device *usbdev, uint8_t ep);
  private:
    /// Size of endpoints.
    static const int ep_size_ = UCOO_CONFIG_HAL_USB_EP_SIZE;
    /// Number of streams (also interfaces or pair of endpoints).
    static const int stream_nb_ = UCOO_CONFIG_HAL_USB_STREAM_NB;
    /// Pointer to the one and only instance.
    static UsbStreamControl *instance_;
    /// Whether device is currently configured.
    bool configured_;
    /// Internal RX buffer type.
    struct RxBuffer
    {
        char buf[ep_size_];
        int size, offset;
        RxBuffer (void);
    };
    /// Internal RX buffer, one per stream.
    RxBuffer rx_buffer_[stream_nb_];
    friend class UsbStream;
};

/// One USB stream, instantiated for each stream, interface, or pair of
/// endpoints.
class UsbStream : public Stream
{
  public:
    /// Construct from control object and stream index.
    UsbStream (UsbStreamControl &control, int num);
    /// See Stream::read.
    int read (char *buf, int count);
    /// See Stream::write.
    int write (const char *buf, int count);
    /// See Stream::poll.
    int poll ();
  private:
    /// Reference to control object.
    UsbStreamControl &control_;
    /// Stream index.
    int num_;
};

} // namespace ucoo

#endif // ucoolib_hal_usb_usb_stm32_hh
