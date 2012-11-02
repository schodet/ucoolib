#ifndef ucoolib_hal_usb_usb_stm32_hh
#define ucoolib_hal_usb_usb_stm32_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
//
// APBTeam:
//        Web: http://apbteam.org/
//      Email: team AT apbteam DOT org
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// }}}
#include "ucoolib/intf/stream.hh"
#include "ucoolib/common.hh"

#include "config/hal/usb.hh"

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
    static void set_config (uint16_t wValue);
    /// Called by USB stack when a frame is received on a OUT endpoint.
    static void rx_callback (uint8_t ep);
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
