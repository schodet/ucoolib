#ifndef ucoolib_dev_avrisp_avrisp_frame_hh
#define ucoolib_dev_avrisp_avrisp_frame_hh
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
#include "avrisp_proto.hh"

#include "ucoolib/intf/stream.hh"

#include "config/dev/avrisp.hh"

namespace ucoo {

/// State of frame decoding.
enum AvrIspFrameState
{
    /// Start state, nothing decoded.
    AVRISP_FRAME_STATE_START,
    /// After message started, waiting sequence number.
    AVRISP_FRAME_STATE_WAIT_SEQ,
    /// Waiting first length byte.
    AVRISP_FRAME_STATE_WAIT_LEN_MSB,
    /// Waiting second length byte.
    AVRISP_FRAME_STATE_WAIT_LEN_LSB,
    /// Waiting token.
    AVRISP_FRAME_STATE_WAIT_TOKEN,
    /// Reading data.
    AVRISP_FRAME_STATE_DATA,
    /// Waiting checksum.
    AVRISP_FRAME_STATE_WAIT_CKSUM,
};

/// Transfer data between a Stream and AvrIspProto which works frame by frame.
class AvrIspFrame
{
  public:
    /// Constructor.
    AvrIspFrame (AvrIspProto &proto);
    /// Read & write from stream.  If operation on stream blocks, this will
    /// block too.
    void read_and_write (Stream &stream);
  private:
    /// Handle received char.
    void accept_char (uint8_t c);
    /// Send frame in buffer (actually just setup, work is done in
    /// read_and_write()).
    void send_frame (int len);
    /// Process found received frame.
    void process ();
  private:
    /// Frame based ISP protocol handler.
    AvrIspProto &proto_;
    /// Current state.
    AvrIspFrameState state_;
    /// Message sequence number.
    uint8_t seq_;
    /// Message length.
    int len_;
    /// Checksum current value.
    uint8_t cksum_;
    /// Buffer to store frame until validated.
    uint8_t buffer_[UCOO_CONFIG_DEV_AVRISP_FRAME_BUFFER_SIZE];
    /// Used buffer length.
    int buffer_len_;
    /// Index to first byte to send from buffer, or -1 if not sending.
    int buffer_send_index_;
    /// Size of the frame header.
    static const uint8_t proto_head_ = 5;
    /// Size of the frame tail.
    static const uint8_t proto_tail_ = 1;
};

} // namespace ucoo

#endif // ucoolib_dev_avrisp_avrisp_frame_hh
