#ifndef ucoolib_intf_i2c_hh
#define ucoolib_intf_i2c_hh
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
#include "ucoolib/common.hh"

namespace ucoo {

/// Master side of I2C interface.
class I2cMaster
{
  public:
    /// Master transfer status, any other value is the transfer size in case
    /// of success.
    enum Status
    {
        /// Transfer is still ongoing.
        STATUS_BUSY = -1,
        /// Transfer finished with error.
        STATUS_ERROR = 0,
    };
    /// Callback used when master transfer is finised.
    class FinishedHandler
    {
      public:
        /// Called when transfer is finished, may be called in interrupt
        /// context.  I2C send and recv can be called from this handler.
        virtual void finished (int status) = 0;
    };
  public:
    /// Send data to selected slave.  This is asynchronous, you will have to
    /// poll status or use a callback to determine if transfer is finished.
    /// Input buffer is not copied and should be kept valid until transfer is
    /// finished.
    ///
    /// If asynchronous transfer is not supported, this will block.
    virtual void send (uint8_t addr, const char *buf, int count) = 0;
    /// Receive data from selected slave.  This is asynchronous, you will have
    /// to poll status or use a callback to determine if transfer is finished.
    /// Buffer is directly written and should be kept valid until transfer is
    /// finished.
    ///
    /// If asynchronous transfer is not supported, this will block.
    virtual void recv (uint8_t addr, char *buf, int count) = 0;
    /// Return last transfer status.
    virtual int status () = 0;
    /// Wait until transfer is finished and return status.
    virtual int wait () = 0;
    /// Register a handler called when transfer is finished.
    void register_finished (FinishedHandler &finished_handler)
        { finished_handler_ = &finished_handler; }
  protected:
    /// Default constructor.
    I2cMaster () : finished_handler_ (0) { }
  protected:
    /// Handler called when transfer is finished.
    FinishedHandler *finished_handler_;
};

/// Slave side of I2C interface.
class I2cSlave
{
  public:
    /// Data source and sink.
    ///
    /// When master requests data from a slave or send data to it, slave is
    /// expected to reply as quickly as possible as the bus is stalled until a
    /// response is done.
    ///
    /// This interface provides data to send to master and receives data from
    /// master.  It has to be fast and may be run under an interrupt context.
    class DataHandler
    {
      public:
        /// Request data to send to master, should write to provided buffer of
        /// size COUNT and return the writen size.
        virtual int to_send (char *buf, int count) = 0;
        /// Provide data sent by master, should make a copy if needed as data will
        /// be discarded.
        virtual void to_recv (const char *buf, int count) = 0;
    };
  public:
    /// Register data source and sink, along with 7 bit address (in MSB).
    virtual void register_data (uint8_t addr, DataHandler &data_handler) = 0;
  protected:
    /// Default constructor.
    I2cSlave () { }
};

/// Both side of I2C interface.
class I2c : public I2cMaster, public I2cSlave
{
  protected:
    /// Default constructor.
    I2c () { }
};

} // namespace ucoo

#endif // ucoolib_intf_i2c_hh
