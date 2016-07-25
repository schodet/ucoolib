#ifndef ucoo_hal_usb_usb_cdc_hh
#define ucoo_hal_usb_usb_cdc_hh
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
#include "ucoo/hal/usb/usb_application.hh"
#include "ucoo/hal/usb/usb_desc.hh"
#include "ucoo/hal/usb/usb_cdc_def.hh"
#include "ucoo/intf/stream.hh"
#include "ucoo/utils/buffer.hh"

#include "config/ucoo/hal/usb.hh"

namespace ucoo {

/// Return a default device descriptor for single CDC ACM device.
details::UsbDesc
usb_cdc_default_device_desc ();

/// Return a default configuration descriptor for single CDC ACM device.
details::UsbDesc
usb_cdc_default_configuration_desc ();

/// Handle a CDC ACM device, in our case a simple serial port.
class UsbApplicationCdcAcm : public UsbApplicationWithSetup, public Stream
{
  public:
    UsbApplicationCdcAcm (UsbDriver &driver)
        : UsbApplicationWithSetup (driver) { }
    SetupResult handle_setup (uint16_t request_n_type,
                              uint16_t value, uint16_t index,
                              uint16_t length) override;
    void handle_configuration_set (uint8_t configuration) override;
    void ep_handle_out (uint8_t ep_address, int bcnt) override;
    void ep_handle_in (uint8_t ep_address) override;
    int read (char *buf, int count) override;
    int write (const char *buf, int count) override;
    int poll () override;
  protected:
    void recv_done () override;
  private:
    /// Send serial state over notification end point.
    void send_serial_state (bool active);
  private:
    /// Line coding being received.
    UsbCdcLineCoding line_coding_;
    /// Whether serial port is active (always the case now).
    static const bool active_ = true;
    /// RX buffer.
    Buffer<char, CONFIG_UCOO_HAL_USB_EP_SIZE * 2> rx_buffer_;
    /// TX buffer.
    Buffer<char, CONFIG_UCOO_HAL_USB_EP_SIZE * 2> tx_buffer_;
    /// Is ready to exchange data?
    bool configured_ = false;
    /// Was serial state requested?
    bool send_serial_state_ = false;
};

} // namespace ucoo

#endif // ucoo_hal_usb_usb_cdc_hh
