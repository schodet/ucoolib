#ifndef ucoo_hal_sdram_sdram_stm32f4_hh
#define ucoo_hal_sdram_sdram_stm32f4_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2015 Nicolas Schodet
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
#include "ucoo/hal/gpio/gpio.hh"
#include "ucoo/common.hh"

#include <initializer_list>

namespace ucoo {

/// Configure an attached SDRAM memory.
class Sdram
{
public:
    struct Io {
        GpioPort &port;
        uint16_t pins;
    };
    struct Param {
        /// FMC bank connected to SDRAM.
        int bank;
        /// Clock frequency.
        int clock_hz;
        /// CAS latency (clock cycles).
        int cas;
        /// Number of internal banks.
        int banks;
        /// Width of data bus.
        int bits;
        /// Number of bits in row address.
        int row_bits;
        /// Number of bits in column address.
        int col_bits;
        /// Row to column delay (clock cycles).  Delay between an activate
        /// command and a read/write.
        int trcd;
        /// Row precharge delay (clock cycles).  Delay between a precharge
        /// command and another command.
        int trp;
        /// Recovery delay (clock cycles).  Delay between a write and a
        /// precharge command.  TWR >= TRAS - TRCD and TWR >= TRC - TRCD -
        /// TRP.
        int twr;
        /// Row cycle delay (clock cycles).  Delay between a refresh command
        /// and an activate command or two consecutive refresh commands.
        int trc;
        /// Self refresh time (clock cycles).
        int tras;
        /// Exit self refresh delay (clock cycles).
        int txsr;
        /// Load mode register to active or refresh delay (clock cycles).
        int tmrd;
        /// Refresh period.
        int tref_ms;
        /// Delay after clock initialisation.
        int init_clock_delay_us;
        /// Number of auto refresh at initialisation.
        int init_auto_refresh;
    };
public:
    /// Constructor, store parameters.
    Sdram (std::initializer_list<Io> ios, const Param &params);
    /// Enable SDRAM controller.
    void enable ();
    /// Get base address.
    void *addr () const;
    /// Get size.
    int size () const;
private:
    /// GPIO used for SDRAM signals.
    std::initializer_list<Io> ios_;
    /// SDRAM parameters.
    const Param &params_;
};

} // namespace ucoo

#endif // ucoo_hal_sdram_sdram_stm32f4_hh
