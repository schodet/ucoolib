#ifndef ucoo_hal_usb_usb_dwc_otg_stm32_hh
#define ucoo_hal_usb_usb_dwc_otg_stm32_hh
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
#include "ucoo/hal/usb/usb_driver.hh"

namespace ucoo {

/// Low level USB driver.
class UsbDriverDwcOtg : public UsbDriver
{
  public:
    enum
    {
        END_POINT_NB = 4,
    };
    enum class Instance
    {
        OTG_FS,
        OTG_HS,
    };
  public:
    /// Constructor for a USB OTG instance.
    template<typename DeviceDesc, typename ConfigurationDesc,
        typename... StringDescs>
    UsbDriverDwcOtg (Instance inst,
                     const DeviceDesc &device_desc,
                     const ConfigurationDesc &configuration_desc,
                     const details::UsbDescs<StringDescs...> &string_descs);
    /// Destructor, disable.
    ~UsbDriverDwcOtg () { disable (); }
    void enable () override;
    void disable () override;
    int ep_write (uint8_t address, const char *data, int size) override;
    void ep_write_ready (uint8_t address) override;
    int ep_read (uint8_t address, char *data, int size) override;
    void ep_read_ready (uint8_t address, int size) override;
    void ep_stall (uint8_t address, bool stall = true) override;
    void set_address (uint8_t address) override;
  protected:
    void ep_setup (uint8_t address, EpType type, int size) override;
  public:
    void isr ();
  private:
    void isr_rx_fifo ();
  private:
    Instance inst_;
    bool enabled_ = false;
    const int rx_fifo_size_ = 512;
    int rx_ep_ = -1;
    int rx_length_ = -1;
    int ep_out_size_[END_POINT_NB];
    int ep_in_size_[END_POINT_NB];
    uint32_t ep_out_doeptsiz_[END_POINT_NB];
    int fifo_used_ = 0;
    uint32_t setup_buf_[2];
};

template<typename DeviceDesc, typename ConfigurationDesc,
    typename... StringDescs>
UsbDriverDwcOtg::UsbDriverDwcOtg (
    Instance inst,
    const DeviceDesc &device_desc,
    const ConfigurationDesc &configuration_desc,
    const details::UsbDescs<StringDescs...> &string_descs)
    : UsbDriver (device_desc, configuration_desc, string_descs),
      inst_ (inst)
{
}

} // namespace ucoo

#endif // ucoo_hal_usb_usb_dwc_otg_stm32_hh
