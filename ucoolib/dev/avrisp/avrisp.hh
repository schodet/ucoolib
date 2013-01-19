#ifndef ucoolib_dev_avrisp_avrisp_hh
#define ucoolib_dev_avrisp_avrisp_hh
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

/// Results returned by ISP functions.
enum AvrIspResult
{
    /// Operation success.
    AVRISP_OK,
    /// Operation failed.
    AVRISP_FAILED,
    /// Operation failed on a timeout.
    AVRISP_TIMEOUT,
};

/// ISP Programming mode.
enum AvrIspMode
{
    /// Use page programming.
    AVRISP_MODE_PAGE = 1,
    /// Use simple delay for word programming.
    AVRISP_MODE_WORD_DELAY = 2,
    /// Use value polling if possible for word programming, fall back to delay
    /// if not possible.
    AVRISP_MODE_WORD_VALUE = 4,
    /// Use RDY/BSY polling for word programming.
    AVRISP_MODE_WORD_RDY_BSY = 8,
    /// Use simple delay for page programming.
    AVRISP_MODE_PAGE_DELAY = 16,
    /// Use value polling if possible for page programming, fall back to delay
    /// if not possible.
    AVRISP_MODE_PAGE_VALUE = 32,
    /// Use RDY/BSY polling for page programming.
    AVRISP_MODE_PAGE_RDY_BSY = 64,
    /// Write page at end of transfer.  Used for big pages which must be
    /// transfered in several packets.
    AVRISP_MODE_PAGE_WRITE = 128,
};

/// Interface to hardware connection.
class AvrIspIntf
{
  public:
    /// Send and receive one byte through SPI interface.
    virtual uint8_t send_and_recv (uint8_t tx) = 0;
    /// Enable programming:
    /// - set RESET and SCK to low,
    /// - power on, or if not possible, do a positive RESET pulse,
    /// - enable SPI, mode 0.
    virtual void enable (uint8_t sck_duration_us) = 0;
    /// Disable programming:
    /// - disable SPI,
    /// - release RESET,
    /// - power off if desired.
    virtual void disable () = 0;
    /// Do a pulse on SCK.  This is used to try to resynchronise.
    virtual void sck_pulse () = 0;
};

/// AVR serial programming.
class AvrIsp
{
  public:
    /// Constructor.
    AvrIsp (AvrIspIntf &intf);
    /// Enable SPI port and enter programing mode.
    /// - sck_duration_us: SCK period.
    /// - timeout_ms: command time-out, unused.
    /// - stab_delay_ms: stabilisation delay once device is reseted and SPI
    ///   initialised.
    /// - cmd_exe_delay_ms: delay for the command execution.
    /// - synch_loops: number of synchronisation loops (there is no start of
    ///   frame in SPI).
    /// - byte_delay_ms: delay between each byte.
    /// - poll_value: value to read to consider the sequence is correct.
    /// - poll_index: transfer index at which the poll_value must be read.
    /// - cmd: command bytes to be transfered.
    /// - returns: AVRISP_OK or AVRISP_FAILED if no synchronisation.
    AvrIspResult enter_progmode (uint8_t sck_duration_us,
                                 uint8_t timeout_ms, uint8_t stab_delay_ms,
                                 uint8_t cmd_exe_delay_ms,
                                 uint8_t synch_loops, uint8_t byte_delay_ms,
                                 uint8_t poll_value, uint8_t poll_index,
                                 const uint8_t cmd[4]);
    /// Leave programming mode and disable SPI port.
    /// - pre_delay_ms: delay before disabling.
    /// - post_delay_ms: delay after disabling.
    void leave_progmode (uint8_t pre_delay_ms, uint8_t post_delay_ms);
    /// Load programing address.
    /// - addr: address to load, bit 31 means the device has more than 64k
    ///   words and extended addressing should be used.
    void load_address (uint32_t addr);
    /// Chip full erase.
    /// - erase_delay_ms: delay to ensure the erase is finished.
    /// - poll_method: use delay (0) or RDY/BSY polling (1).
    /// - cmd: chip erase command.
    /// - returns: AVRISP_OK or AVRISP_TIMEOUT if using polling and it
    ///   timed-out.
    AvrIspResult chip_erase (uint8_t erase_delay_ms, uint8_t poll_method,
                             const uint8_t cmd[4]);
    /// Start a program command.
    /// - num_bytes: total number of bytes to program.
    /// - returns: AVRISP_OK or AVRISP_FAILED for bad parameters.
    /// See class for other parameters meaning.
    AvrIspResult program_begin (uint16_t num_bytes, uint8_t mode,
                                uint8_t delay_ms, uint8_t cmd_write_mem,
                                uint8_t cmd_write_page, uint8_t cmd_read_mem,
                                const uint8_t poll[2], uint8_t flash);
    /// Start a flash memory program command.
    /// - num_bytes: total number of bytes to program.
    /// - returns: AVRISP_OK or AVRISP_FAILED for bad parameters.
    /// See class for other parameters meaning.
    AvrIspResult program_flash_begin (uint16_t num_bytes, uint8_t mode,
                                      uint8_t delay_ms, uint8_t cmd_write_mem,
                                      uint8_t cmd_write_page,
                                      uint8_t cmd_read_mem,
                                      const uint8_t poll[2]);
    /// Start a EEPROM memory program command.
    /// - num_bytes: total number of bytes to program.
    /// - returns: AVRISP_OK or AVRISP_FAILED for bad parameters.
    /// See class for other parameters meaning.
    AvrIspResult program_eeprom_begin (uint16_t num_bytes,
                                       uint8_t mode,
                                       uint8_t delay_ms,
                                       uint8_t cmd_write_mem,
                                       uint8_t cmd_write_page,
                                       uint8_t cmd_read_mem,
                                       const uint8_t poll[2]);
    /// Provide data for memory programming.  Data should be given by even
    /// sized packs or loading of word addressed data will fail.
    /// - returns: AVRISP_OK, AVRISP_FAILED for bad parameters or
    ///   AVRISP_TIMEOUT if using polling and it timed-out.
    AvrIspResult program_continue (const uint8_t *data, uint16_t size);
    /// End program command.
    /// - returns: AVRISP_OK, AVRISP_FAILED if too early or AVRISP_TIMEOUT if
    ///   using polling and it timed-out.
    AvrIspResult program_end ();
    /// Start a read command.
    /// - num_bytes: total number of bytes to program.
    /// - returns: AVRISP_OK or AVRISP_FAILED for bad parameters.
    /// See class for other parameters meaning.
    AvrIspResult read_begin (uint16_t num_bytes, uint8_t cmd_read_mem,
                             uint8_t flash);
    /// Start a flash memory read command.
    /// - num_bytes: total number of bytes to program.
    /// - returns: AVRISP_OK or AVRISP_FAILED for bad parameters.
    /// See class for other parameters meaning.
    AvrIspResult read_flash_begin (uint16_t num_bytes, uint8_t cmd_read_mem);
    /// Start a EEPROM memory read command.
    /// - num_bytes: total number of bytes to program.
    /// - returns: AVRISP_OK or AVRISP_FAILED for bad parameters.
    /// See class for other parameters meaning.
    AvrIspResult read_eeprom_begin (uint16_t num_bytes, uint8_t cmd_read_mem);
    /// Get data from read memory.  Data should be read by even sized packs or
    /// loading of word addressed data will fail.
    /// - returns: AVRISP_OK or AVRISP_FAILED for bad parameters.
    AvrIspResult read_continue (uint8_t *data, uint16_t size);
    /// End read command.
    /// - returns: AVRISP_OK or AVRISP_FAILED if too early.
    AvrIspResult read_end ();
    /// Program miscellaneous memory (fuse, lock).
    /// - cmd: program command.
    void program_misc (const uint8_t cmd[4]);
    /// Read miscellaneous memory (fuse, lock, signature, osccal).
    /// - ret_addr: transfer index at which the return value must be read.
    /// - cmd: read command.
    uint8_t read_misc (uint8_t ret_addr, const uint8_t cmd[4]);
    /// Generic SPI access.
    /// - num_tx: number of bytes to transmit.
    /// - num_rx: number of bytes to receive.
    /// - rx_start: start reception after this number of transmitted bytes.
    /// - dout: buffer to read sent bytes.
    /// - din: buffer to write received bytes.
    /// Limitation: no support for doing this in several chunks as memory
    /// programing and reading.
    void multi (uint8_t num_tx, uint8_t num_rx, uint8_t rx_start,
                const uint8_t *dout, uint8_t *din);
  private:
    /// Transmit a 16 bit data.
    void spi_tx_16 (uint16_t data);
    /// Transmit a 32 bit data and return last byte received.  This is used for
    /// RDY/BSY polling.
    uint8_t spi_tx_32 (uint32_t data);
  private:
    /// Hardware interface.
    AvrIspIntf &intf_;
    /// Current address.
    uint16_t addr_;
    /// Current extended address.
    uint8_t ext_addr_;
    /// Extended address loaded in device.
    uint8_t last_ext_addr_;
    /// Whether the device needs extended addressing.
    uint8_t larger_than_64k_;
    /// Programming start address (used for page program command).
    uint16_t start_addr_;
    /// Number of bytes left to process.
    uint16_t bytes_left_;
    /// Programming mode.
    uint8_t mode_;
    /// Programming delay if no other poll method.
    uint8_t delay_ms_;
    /// Write Program Memory or Load Page.
    uint8_t cmd_write_mem_;
    /// Write Page.
    uint8_t cmd_write_page_;
    /// Read Program Memory.
    uint8_t cmd_read_mem_;
    /// Value read until memory is programmed for value polling.
    uint8_t poll_[2];
    /// Whether the page can be polled (if almost one byte is not poll[0]).
    /// This is non zero if true, with LSB = 1 for high byte.
    uint8_t pollable_;
    /// Address to use for polling.
    uint16_t poll_addr_;
    /// Whether this is a flash memory.
    uint8_t flash_;
};

} // namespace ucoo

#endif // ucoolib_dev_avrisp_avrisp_hh
