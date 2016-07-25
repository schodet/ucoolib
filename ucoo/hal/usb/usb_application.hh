#ifndef ucoo_hal_usb_usb_application_hh
#define ucoo_hal_usb_usb_application_hh
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
#include "ucoo/hal/usb/usb_desc.hh"

#include "ucoo/common.hh"

namespace ucoo {

class UsbDriver;

/// Class which handles application specific controls and data exchanges.
class UsbApplication
{
  public:
    /// Response from a setup handler.
    enum class SetupResult
    {
        /// Setup not handled, should continue with next registered
        /// application.
        NotHandled,
        /// Setup handled, this application is now responsible for any
        /// following IN or OUT.
        Handled,
        /// Setup error, STALL the control end point.
        Error
    };
  public:
    /// Handle setup message received on control end point. If message is
    /// handled, this application is automatically made responsible for IN and
    /// OUT on this end point. It can use ep_write right now (if it does not,
    /// it will not receive any ep_handle_in event).
    /// On error the control end point is STALLed.
    virtual SetupResult handle_setup (uint16_t request_n_type,
                                      uint16_t value, uint16_t index,
                                      uint16_t length) = 0;
    /// Handle configuration set, should setup any used end points.
    virtual void handle_configuration_set (uint8_t configuration) = 0;
    /// Handle data received on an OUT end point.  The application must use
    /// ep_read once to receive data.
    virtual void ep_handle_out (uint8_t ep_address, int bcnt) = 0;
    /// Handle data needed to be transmitted on an IN end point.  The
    /// application can use ep_write once to transmit data.  Else the end
    /// point is left in a NAK state.
    virtual void ep_handle_in (uint8_t ep_address) = 0;
  protected:
    /// Register application.
    UsbApplication (UsbDriver &driver);
  protected:
    UsbDriver &driver_;
};

/// Helper for SETUP handling.
class UsbApplicationWithSetup : public UsbApplication
{
  public:
    /// SETUP stage, to be used as a SETUP FSM.
    enum class SetupStage
    {
        /// Idle, waiting for a SETUP frame.
        Idle,
        /// Host to device, waiting for an OUT frame.
        HostToDeviceOUT,
        /// Host to device, status IN frame ready to send.
        HostToDeviceStatusIN,
        /// Device to host, IN frame ready to send.
        DeviceToHostIN,
        /// Device to host, waiting for status OUT frame.
        DeviceToHostStatusOUT,

    };
  public:
    UsbApplicationWithSetup (UsbDriver &driver)
        : UsbApplication (driver) { }
    void ep_handle_out (uint8_t ep_address, int bcnt) override;
    void ep_handle_in (uint8_t ep_address) override;
  protected:
    /// Send status IN.
    void send_status ();
    /// Enqueue data to send in response to SETUP.
    void send (const char *data, int data_size);
    /// Prepare to receive data after SETUP.
    void recv (char *data, int data_size);
    /// Data reception done after SETUP.
    virtual void recv_done ();
  private:
    SetupStage setup_stage_ = SetupStage::Idle;
    char *data_ = nullptr;
    int data_size_ = 0;
};

/// Handle basic USB protocol.
class UsbApplicationBasic : public UsbApplicationWithSetup
{
  public:
    template<typename DeviceDesc, typename ConfigurationDesc,
        typename... StringDescs>
    UsbApplicationBasic (UsbDriver &driver,
                         const DeviceDesc &device_desc,
                         const ConfigurationDesc &configuration_desc,
                         const details::UsbDescs<StringDescs...> &string_descs);
    SetupResult handle_setup (uint16_t request_n_type,
                              uint16_t value, uint16_t index,
                              uint16_t length) override;
    void handle_configuration_set (uint8_t configuration) override;
  private:
    /// Send a descriptor.
    SetupResult send_descriptor (uint8_t desc_type, uint8_t desc_index,
                                 uint16_t language, uint16_t length);
  private:
    const details::UsbDesc device_desc_;
    const details::UsbDesc configuration_desc_;
    const details::UsbDesc *string_descs_;
    const int string_descs_nb_;
    SetupStage setup_stage_ = SetupStage::Idle;
    const char *to_send_ = nullptr;
    int to_send_size_ = 0;
};

template<typename DeviceDesc, typename ConfigurationDesc,
    typename... StringDescs>
UsbApplicationBasic::UsbApplicationBasic (
    UsbDriver &driver,
    const DeviceDesc &device_desc,
    const ConfigurationDesc &configuration_desc,
    const details::UsbDescs<StringDescs...> &string_descs)
    : UsbApplicationWithSetup (driver),
      device_desc_ (device_desc), configuration_desc_ (configuration_desc),
      string_descs_ (reinterpret_cast<const details::UsbDesc *> (&string_descs)),
      string_descs_nb_ (sizeof (string_descs) / sizeof (details::UsbDesc))
{
}

} // namespace ucoo

#endif // ucoo_hal_usb_usb_application_hh
