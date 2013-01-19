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

#include "ucoolib/utils/delay.hh"

namespace ucoo {

/// Command used to load extended address.
static const uint8_t avrisp_cmd_load_extended_address = 0x4d;
/// Bit to address high byte.
static const uint8_t avrisp_cmd_high_byte = 1 << 3;
/// Extra EEPROM programming delay.
static const int avrisp_eeprom_delay_ms = 2;
/// Miscellaneous read delay.
static const int avrisp_misc_read_delay_ms = 5;

AvrIsp::AvrIsp (AvrIspIntf &intf)
    : intf_ (intf)
{
}

AvrIspResult
AvrIsp::enter_progmode (uint8_t sck_duration_us, uint8_t timeout_ms,
                        uint8_t stab_delay_ms, uint8_t cmd_exe_delay_ms,
                        uint8_t synch_loops, uint8_t byte_delay_ms,
                        uint8_t poll_value, uint8_t poll_index,
                        const uint8_t cmd[4])
{
    int i;
    uint8_t tmp1, tmp2;
    // Reset context.
    ext_addr_ = 0;
    last_ext_addr_ = 0xff;
    larger_than_64k_ = 0;
    bytes_left_ = 0;
    // Enable SPI.
    intf_.enable (sck_duration_us);
    delay_ms (stab_delay_ms);
    // Limit the number of loops.
    if (synch_loops > 48)
        synch_loops = 48;
    // Minimum byte delay.
    if (byte_delay_ms < 1)
        byte_delay_ms = 1;
    // Synchronisation loops.
    for (i = 0; i < synch_loops; i++)
    {
        delay_ms (cmd_exe_delay_ms);
        intf_.send_and_recv (cmd[0]);
        delay_ms (byte_delay_ms);
        intf_.send_and_recv (cmd[1]);
        delay_ms (byte_delay_ms);
        tmp1 = intf_.send_and_recv (cmd[2]);
        delay_ms (byte_delay_ms);
        tmp2 = intf_.send_and_recv (cmd[3]);
        // Test success.
        if (poll_index == 0
            || (poll_index == 3 && tmp1 == poll_value)
            || (poll_index != 3 && tmp2 == poll_value))
            return AVRISP_OK;
        // Else, new try.
        intf_.sck_pulse ();
        delay_ms (20);
    }
    return AVRISP_FAILED;
}

void
AvrIsp::leave_progmode (uint8_t pre_delay_ms, uint8_t post_delay_ms)
{
    delay_ms (pre_delay_ms);
    intf_.disable ();
    delay_ms (post_delay_ms);
}

void
AvrIsp::load_address (uint32_t addr)
{
    addr_ = addr & 0xffff;
    ext_addr_ = (addr >> 16) & 0xff;
    last_ext_addr_ = 0xff;
    larger_than_64k_ = (addr & 0x80000000) ? 1 : 0;
}

AvrIspResult
AvrIsp::chip_erase (uint8_t erase_delay_ms, uint8_t poll_method,
                    const uint8_t cmd[4])
{
    int tries;
    intf_.send_and_recv (cmd[0]);
    intf_.send_and_recv (cmd[1]);
    intf_.send_and_recv (cmd[2]);
    intf_.send_and_recv (cmd[3]);
    if (poll_method == 0)
    {
        // Use delay.
        delay_ms (erase_delay_ms);
    }
    else
    {
        // Use RDY/BSY command.
        tries = 150;
        while ((spi_tx_32 (0xf0000000) & 1) && tries)
            tries--;
        if (tries == 0)
            return AVRISP_TIMEOUT;
    }
    return AVRISP_OK;
}

AvrIspResult
AvrIsp::program_begin (uint16_t num_bytes, uint8_t mode, uint8_t delay_ms,
                       uint8_t cmd_write_mem, uint8_t cmd_write_page,
                       uint8_t cmd_read_mem, const uint8_t poll[2],
                       uint8_t flash)
{
    // Set delay bounds.
    if (delay_ms < 4)
        delay_ms = 4;
    if (delay_ms > 32)
        delay_ms = 32;
    // Check data size.
    if (flash && (num_bytes & 1))
        return AVRISP_FAILED;
    // Store parameters.
    start_addr_ = addr_;
    bytes_left_ = num_bytes;
    mode_ = mode;
    delay_ms_ = delay_ms;
    cmd_write_mem_ = cmd_write_mem;
    cmd_write_page_ = cmd_write_page;
    cmd_read_mem_ = cmd_read_mem;
    poll_[0] = poll[0];
    poll_[1] = poll[1];
    pollable_ = 0;
    flash_ = flash;
    return AVRISP_OK;
}

AvrIspResult
AvrIsp::program_flash_begin (uint16_t num_bytes, uint8_t mode,
                             uint8_t delay_ms, uint8_t cmd_write_mem,
                             uint8_t cmd_write_page, uint8_t cmd_read_mem,
                             const uint8_t poll[2])
{
    return program_begin (num_bytes, mode, delay_ms, cmd_write_mem,
                          cmd_write_page, cmd_read_mem, poll, 1);
}

AvrIspResult
AvrIsp::program_eeprom_begin (uint16_t num_bytes, uint8_t mode,
                              uint8_t delay_ms, uint8_t cmd_write_mem,
                              uint8_t cmd_write_page, uint8_t cmd_read_mem,
                              const uint8_t poll[2])
{
    return program_begin (num_bytes, mode, delay_ms, cmd_write_mem,
                          cmd_write_page, cmd_read_mem, poll, 0);
}

AvrIspResult
AvrIsp::program_continue (const uint8_t *data, uint16_t size)
{
    uint16_t i;
    uint8_t read;
    int tries;
    // Check size.
    if (size > bytes_left_ || (flash_ && (size & 1)))
        return AVRISP_FAILED;
    bytes_left_ -= size;
    // Loop on each input bytes.
    for (i = 0; i < size; i++)
    {
        // Check for Load Extended Address.
        if (larger_than_64k_ && last_ext_addr_ != ext_addr_)
        {
            intf_.send_and_recv (avrisp_cmd_load_extended_address);
            intf_.send_and_recv (0x00);
            intf_.send_and_recv (ext_addr_);
            intf_.send_and_recv (0x00);
            last_ext_addr_ = ext_addr_;
        }
        // Write memory/load page, using bit 3 as byte selector.
        if (flash_ && (i & 1))
            intf_.send_and_recv (cmd_write_mem_ | avrisp_cmd_high_byte);
        else
            intf_.send_and_recv (cmd_write_mem_);
        spi_tx_16 (addr_);
        intf_.send_and_recv (data[i]);
        // EEPROM needs more delay.
        if (!flash_)
            delay_ms (avrisp_eeprom_delay_ms);
        // Poll.
        if (!(mode_ & AVRISP_MODE_PAGE))
        {
            // Poll right now.
            tries = 150;
            read = poll_[0];
            if ((mode_ & AVRISP_MODE_WORD_VALUE) && data[i] != read)
            {
                // Poll by reading byte.
                while (read != data[i] && tries)
                {
                    if (flash_ && (i & 1))
                        intf_.send_and_recv (cmd_read_mem_
                                             | avrisp_cmd_high_byte);
                    else
                        intf_.send_and_recv (cmd_read_mem_);
                    spi_tx_16 (addr_);
                    read = intf_.send_and_recv (0x00);
                    tries--;
                }
            }
            else if (mode_ & AVRISP_MODE_WORD_RDY_BSY)
            {
                // RDY/BSY polling.
                while ((spi_tx_32 (0xf0000000) & 1) && tries)
                    tries--;
            }
            else
                // Simple delay.
                delay_ms (delay_ms_);
            if (tries == 0)
                return AVRISP_TIMEOUT;
        }
        else
        {
            // Check for poll method.
            if (!pollable_ && data[i] != poll_[0])
            {
                pollable_ = 0x02 | (i & 1);
                poll_addr_ = addr_;
            }
        }
        // Increment address.
        if (!flash_ || (i & 1))
        {
            addr_++;
            if (addr_ == 0)
                ext_addr_++;
        }
    }
    return AVRISP_OK;
}

AvrIspResult
AvrIsp::program_end ()
{
    uint8_t read;
    int tries;
    // Check size.
    if (bytes_left_ != 0)
        return AVRISP_FAILED;
    // Write page if requested.
    if ((mode_ & (AVRISP_MODE_PAGE | AVRISP_MODE_PAGE_WRITE))
        == (AVRISP_MODE_PAGE | AVRISP_MODE_PAGE_WRITE))
    {
        // Write page.
        intf_.send_and_recv (cmd_write_page_);
        spi_tx_16 (start_addr_);
        intf_.send_and_recv (0x00);
        // EEPROM needs more delay.
        if (!flash_)
            delay_ms (avrisp_eeprom_delay_ms);
        // Poll.
        tries = 150;
        read = poll_[0];
        if ((mode_ & AVRISP_MODE_PAGE_VALUE) && pollable_)
        {
            // Poll by reading byte.
            while (read == poll_[0] && tries)
            {
                if (flash_ && (pollable_ & 1))
                    intf_.send_and_recv (cmd_read_mem_ | avrisp_cmd_high_byte);
                else
                    intf_.send_and_recv (cmd_read_mem_);
                spi_tx_16 (poll_addr_);
                read = intf_.send_and_recv (0x00);
                tries--;
            }
        }
        else if (mode_ & AVRISP_MODE_PAGE_RDY_BSY)
        {
            // RDY/BSY polling.
            while ((spi_tx_32 (0xf0000000) & 1) && tries)
                tries--;
        }
        else
            // Simple delay.
            delay_ms (delay_ms_);
        if (tries == 0)
            return AVRISP_TIMEOUT;
    }
    return AVRISP_OK;
}

AvrIspResult
AvrIsp::read_begin (uint16_t num_bytes, uint8_t cmd_read_mem, uint8_t flash)
{
    // Check data size.
    if (flash && (num_bytes & 1))
        return AVRISP_FAILED;
    // Store parameters.
    bytes_left_ = num_bytes;
    cmd_read_mem_ = cmd_read_mem;
    flash_ = flash;
    return AVRISP_OK;
}

AvrIspResult
AvrIsp::read_flash_begin (uint16_t num_bytes, uint8_t cmd_read_mem)
{
    return read_begin (num_bytes, cmd_read_mem, 1);
}

AvrIspResult
AvrIsp::read_eeprom_begin (uint16_t num_bytes, uint8_t cmd_read_mem)
{
    return read_begin (num_bytes, cmd_read_mem, 0);
}

AvrIspResult
AvrIsp::read_continue (uint8_t *data, uint16_t size)
{
    uint16_t i;
    // Check size.
    if (size > bytes_left_ || (flash_ && (size & 1)))
        return AVRISP_FAILED;
    bytes_left_ -= size;
    // Loop on each bytes.
    for (i = 0; i < size; i++)
    {
        // Check for Load Extended Address.
        if (larger_than_64k_ && last_ext_addr_ != ext_addr_)
        {
            intf_.send_and_recv (avrisp_cmd_load_extended_address);
            intf_.send_and_recv (0x00);
            intf_.send_and_recv (ext_addr_);
            intf_.send_and_recv (0x00);
            last_ext_addr_ = ext_addr_;
        }
        // Read memory, using bit 3 as byte selector.
        if (flash_ && (i & 1))
            intf_.send_and_recv (cmd_read_mem_ | avrisp_cmd_high_byte);
        else
            intf_.send_and_recv (cmd_read_mem_);
        spi_tx_16 (addr_);
        data[i] = intf_.send_and_recv (0x00);
        // Increment address.
        if (!flash_ || (i & 1))
        {
            addr_++;
            if (addr_ == 0)
                ext_addr_++;
        }
    }
    return AVRISP_OK;
}

AvrIspResult
AvrIsp::read_end ()
{
    // Check size.
    if (bytes_left_ != 0)
        return AVRISP_FAILED;
    return AVRISP_OK;
}

void
AvrIsp::program_misc (const uint8_t cmd[4])
{
    uint8_t i;
    for (i = 0; i < 4; i++)
        intf_.send_and_recv (cmd[i]);
}

uint8_t
AvrIsp::read_misc (uint8_t ret_addr, const uint8_t cmd[4])
{
    uint8_t i, read = 0, tmp;
    for (i = 0; i < 4; i++)
    {
        tmp = intf_.send_and_recv (cmd[i]);
        if (i == ret_addr)
            read = tmp;
        delay_ms (avrisp_misc_read_delay_ms);
    }
    return read;
}

void
AvrIsp::multi (uint8_t num_tx, uint8_t num_rx, uint8_t rx_start,
               const uint8_t *dout, uint8_t *din)
{
    uint8_t in, out;
    while (num_tx || num_rx)
    {
        out = 0;
        if (num_tx)
        {
            out = *dout++;
            num_tx--;
        }
        in = intf_.send_and_recv (out);
        if (rx_start)
            rx_start--;
        else if (num_rx)
        {
            *din++ = in;
            num_rx--;
        }
    }
}

void
AvrIsp::spi_tx_16 (uint16_t data)
{
    intf_.send_and_recv ((data >> 8) & 0xff);
    intf_.send_and_recv ((data >> 0) & 0xff);
}

uint8_t
AvrIsp::spi_tx_32 (uint32_t data)
{
    intf_.send_and_recv ((data >> 24) & 0xff);
    intf_.send_and_recv ((data >> 16) & 0xff);
    intf_.send_and_recv ((data >> 8) & 0xff);
    return intf_.send_and_recv ((data >> 0) & 0xff);
}

} // namespace ucoo
