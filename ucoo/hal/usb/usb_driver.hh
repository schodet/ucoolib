#ifndef ucoo_hal_usb_usb_driver_hh
#define ucoo_hal_usb_usb_driver_hh
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

#include "ucoo/utils/trace.hh"
#include "config/ucoo/hal/usb.hh"

namespace ucoo {

extern Trace<CONFIG_UCOO_HAL_USB_TRACE> usb_trace;

/// Low level USB driver.
class UsbDriver
{
  public:
    enum class EpType
    {
        Control,
        Isochronous,
        Bulk,
        Interrupt,
    };
  public:
    /// Constructor.
    template<typename DeviceDesc, typename ConfigurationDesc,
        typename... StringDescs>
    UsbDriver (const DeviceDesc &device_desc,
               const ConfigurationDesc &configuration_desc,
               const details::UsbDescs<StringDescs...> &string_descs);
    /// No copy constructor.
    UsbDriver (const UsbDriver &) = delete;
    /// Register an application, typically done in application constructor.
    void register_application (UsbApplication &app);
    /// Enable hardware.
    virtual void enable () = 0;
    /// Disable hardware.
    virtual void disable () = 0;
    /// Write data to an end point.  Return the number of written bytes,
    /// following bytes will have to be written once there is room in TX FIFO.
    virtual int ep_write (uint8_t address, const char *data, int size) = 0;
    /// Signal application have some data to send.
    virtual void ep_write_ready (uint8_t address) = 0;
    /// Read data from an end point.  Should only be done when data is
    /// signaled to be available.  Return the number of read bytes.
    virtual int ep_read (uint8_t address, char *data, int size) = 0;
    /// Signal application can read data (with maximum buffer size).
    virtual void ep_read_ready (uint8_t address, int size) = 0;
    /// Stall an end point.
    virtual void ep_stall (uint8_t address, bool stall = true) = 0;
    /// Set device address.
    virtual void set_address (uint8_t address) = 0;
    /// Set configuration.
    void set_configuration (int configuration);
    /// Setup an end point, make an application responsible for it.
    void ep_setup (uint8_t address, EpType type, int size, UsbApplication &app);
  protected:
    /// Setup an end point.
    virtual void ep_setup (uint8_t address, EpType type, int size) = 0;
    /// Handle reset, called by driver.
    void handle_reset ();
    /// Handle SETUP, called by driver when received.
    void handle_setup (uint32_t *msg);
    /// Handle OUT, called by driver when received.
    void ep_handle_out (uint8_t address, int bcnt);
    /// Handle IN, called by driver when transmission can be done.
    void ep_handle_in (uint8_t address);
  private:
    enum {
        APPLICATION_MAX = 3,
        END_POINT_MAX = 4,
    };
    /// Registered applications.
    UsbApplication *applications_[APPLICATION_MAX];
    /// Handlers of OUT events.
    UsbApplication *out_handlers_[END_POINT_MAX];
    /// Handlers of IN events.
    UsbApplication *in_handlers_[END_POINT_MAX];
    /// Basic USB protocol application.
    UsbApplicationBasic app_basic_;
    /// Current configuration or -1 if not configured.
    int current_configuration_ = -1;
};

template<typename DeviceDesc, typename ConfigurationDesc,
    typename... StringDescs>
UsbDriver::UsbDriver (const DeviceDesc &device_desc,
                      const ConfigurationDesc &configuration_desc,
                      const details::UsbDescs<StringDescs...> &string_descs)
    : applications_ { nullptr }, out_handlers_ { nullptr },
      in_handlers_ { nullptr },
      app_basic_ (*this, device_desc, configuration_desc, string_descs)
{
}

} // namespace ucoo

#endif // ucoo_hal_usb_usb_driver_hh
