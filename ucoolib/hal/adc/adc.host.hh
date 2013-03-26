#ifndef ucoolib_hal_adc_adc_host_hh
#define ucoolib_hal_adc_adc_host_hh
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
#include "ucoolib/arch/host/host.hh"

namespace ucoo {

class AdcHostShared;

/// Host ADC channel.
class AdcHost : public Adc
{
  public:
    /// Constructor.
    AdcHost (Host &host, const char *name, int resolution);
    /// See Adc::read.
    int read () { return value_; }
    /// See Adc::get_resolution.
    int get_resolution () const { return resolution_; }
  private:
    int resolution_;
    int value_;
    static AdcHostShared *shared_;
    friend class AdcHostShared;
};

} // namespace ucoo

#endif // ucoolib_hal_adc_adc_host_hh
