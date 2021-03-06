#ifndef ucoo_dev_avrisp_avrisp_frame_hh
#define ucoo_dev_avrisp_avrisp_frame_hh
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
#include "avrisp_proto.hh"

#include "ucoo/intf/stream.hh"

#include "config/ucoo/dev/avrisp.hh"

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
    uint8_t buffer_[CONFIG_UCOO_DEV_AVRISP_FRAME_BUFFER_SIZE];
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

#endif // ucoo_dev_avrisp_avrisp_frame_hh
