#ifndef ucoolib_hal_adc_adc_hard_stm32f4_hh
#define ucoolib_hal_adc_adc_hard_stm32f4_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
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
#include "ucoolib/intf/adc.hh"
#include "ucoolib/common.hh"

namespace ucoo {

/// ADC interface.  This control a full ADC, use AdcHardChannel for a single
/// channel.
class AdcHard
{
  public:
    static const int resolution = 1 << 12;
  public:
    /// Constructor for the Nth ADC.
    AdcHard (int n);
    /// Shutdown.
    ~AdcHard ();
    /// Enable, power on.
    void enable ();
    /// Disable.
    void disable ();
    /// Make a single measure.
    int read (int channel);
  private:
    /// ADC index.
    int n_;
    /// ADC base address.
    uint32_t base_;
};

/// Single ADC channel.
class AdcHardChannel : public Adc
{
  public:
    /// Constructor.
    AdcHardChannel (AdcHard &adc, int channel)
        : adc_ (adc), channel_ (channel) { }
    /// See Adc::read.
    int read () { return adc_.read (channel_); }
    /// See Adc::get_resolution.
    int get_resolution () const { return AdcHard::resolution; }
  private:
    AdcHard &adc_;
    int channel_;
};

} // namespace ucoo

#endif // ucoolib_hal_adc_adc_hard_stm32f4_hh
