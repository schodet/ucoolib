#ifndef ucoo_dev_avrisp_avrisp_proto_hh
#define ucoo_dev_avrisp_avrisp_proto_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
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

#endif // ucoo_dev_avrisp_avrisp_proto_hh
