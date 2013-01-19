#ifndef ucoolib_dev_avrisp_avrisp_proto_hh
#define ucoolib_dev_avrisp_avrisp_proto_hh
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
#include "avrisp.hh"

namespace ucoo {

/// Interpret commands in the AVR068 and AVR069 format.  Framing is needed for
/// AVR068 and provided by AvrIspFrame.
class AvrIspProto
{
  public:
    /// Constructor.
    AvrIspProto (AvrIsp &isp);
    /// Accept a frame to interpret.  Return the response length provided in
    /// the same buffer.  The buffer should be large enough to store the
    /// largest frame used by AVR06[89] protocol.
    int accept (uint8_t *data, int len);
  private:
    /// Ref to ISP class.
    AvrIsp &isp_;
    /// SCK period.
    uint8_t sck_duration_us_;
};

} // namespace ucoo

#endif // ucoolib_dev_avrisp_avrisp_proto_hh
