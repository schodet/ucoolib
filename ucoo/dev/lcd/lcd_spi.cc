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
#include "ucoo/dev/lcd/lcd_spi.hh"

#include "ucoo/common.hh"
#include "ucoo/utils/delay.hh"

#include <cstring>

namespace ucoo {

enum
{
    LCD_CLEAR = 0x01,
    LCD_HOME = 0x02,
    LCD_ENTRY_MODE = 0x04,
    LCD_ENTRY_MODE__INCR = 0x02,
    LCD_ENTRY_MODE__SHIFT = 0x01,
    LCD_DISPLAY_ON_OFF = 0x08,
    LCD_DISPLAY_ON_OFF__ON = 0x04,
    LCD_DISPLAY_ON_OFF__CURSOR = 0x02,
    LCD_DISPLAY_ON_OFF__BLINK = 0x01,
    LCD_SHIFT = 0x10,
    LCD_SHIFT__DISPLAY = 0x08,
    LCD_SHIFT__RIGHT = 0x04,
    LCD_FUNCTION_SET = 0x20,
    LCD_FUNCTION_SET__8BIT = 0x10,
    LCD_FUNCTION_SET__2LINES = 0x08,
    LCD_FUNCTION_SET__FONT = 0x04,
    LCD_FUNCTION_SET__INSTRUCTION_TABLE_1 = 0x01,
    LCD_SET_CGRAM_ADDRESS = 0x40,
    LCD_SET_DDRAM_ADDRESS = 0x80,

    LCD_IS1_BIAS_SET = 0x14,
    LCD_IS1_BIAS_SET__1_4_BIAS = 0x08,
    LCD_IS1_BIAS_SET__1_5_BIAS = 0x00,
    LCD_IS1_POWER_CONTROL = 0x50,
    LCD_IS1_POWER_CONTROL__BOOST_ON = 0x04,
    LCD_IS1_POWER_CONTROL__CONTRAST_C5 = 0x02,
    LCD_IS1_POWER_CONTROL__CONTRAST_C4 = 0x01,
    LCD_IS1_FOLLOWER_CONTROL = 0x60,
    LCD_IS1_FOLLOWER_CONTROL__ON = 0x08,
    LCD_IS1_CONTRAST_SET = 0x70,
    LCD_IS1_CONTRAST_SET__CONTRAST_C3 = 0x08,
    LCD_IS1_CONTRAST_SET__CONTRAST_C2 = 0x04,
    LCD_IS1_CONTRAST_SET__CONTRAST_C1 = 0x02,
    LCD_IS1_CONTRAST_SET__CONTRAST_C0 = 0x01,
};

LcdSpi::~LcdSpi ()
{
    disable ();
}

void
LcdSpi::enable ()
{
    enabled_ = true;
    // Enable bus.
    cs_.output ();
    cs_.set ();
    rs_.output ();
    spi_.enable (spi_speed_hz_, SPI_MODE_3);
    // Initialise display.
    char buf[] = {
        LCD_FUNCTION_SET | LCD_FUNCTION_SET__8BIT | LCD_FUNCTION_SET__2LINES
            | LCD_FUNCTION_SET__INSTRUCTION_TABLE_1,
        LCD_IS1_BIAS_SET | LCD_IS1_BIAS_SET__1_5_BIAS,
        LCD_IS1_POWER_CONTROL | LCD_IS1_POWER_CONTROL__BOOST_ON | 0x1,
        LCD_IS1_FOLLOWER_CONTROL | LCD_IS1_FOLLOWER_CONTROL__ON | 5,
        LCD_IS1_CONTRAST_SET | 0x8,
        LCD_FUNCTION_SET | LCD_FUNCTION_SET__8BIT | LCD_FUNCTION_SET__2LINES,
        LCD_DISPLAY_ON_OFF | LCD_DISPLAY_ON_OFF__ON,
        LCD_CLEAR,
        LCD_ENTRY_MODE | LCD_ENTRY_MODE__INCR,
    };
    send (buf, sizeof (buf), true, true);
    cur_line_ = 0;
}

void
LcdSpi::disable ()
{
    if (enabled_)
    {
        enabled_ = false;
        // Turn off display.
        char buf[] = { LCD_DISPLAY_ON_OFF };
        send (buf, sizeof (buf), true);
        // Disable bus.
        spi_.disable ();
        rs_.input ();
        cs_.input ();
    }
}

void
LcdSpi::puts (const char *str)
{
    while (*str)
    {
        const char *end = str + strcspn (str, "\n\r");
        if (end != str)
            send (str, end - str);
        str = end;
        switch (*str)
        {
        case '\n':
            go (cur_line_ + 1, 0);
            str++;
            break;
        case '\r':
            go (cur_line_, 0);
            str++;
            break;
        default:
            break;
        }
    }
}

void
LcdSpi::go (int line, int column)
{
    line = line % lines_;
    int addr = line * 0x40 + column;
    char buf[] = { static_cast<char> (LCD_SET_DDRAM_ADDRESS | addr) };
    send (buf, sizeof (buf), true);
    cur_line_ = line;
}

void
LcdSpi::clear ()
{
    char buf[] = { LCD_CLEAR };
    send (buf, sizeof (buf), true, true);
}

int
LcdSpi::get_lines () const
{
    return lines_;
}

int
LcdSpi::get_columns () const
{
    return columns_;
}

void
LcdSpi::send (const char *buf, int count, bool command, bool long_delay)
{
    assert (enabled_);
    rs_.set (!command);
    cs_.reset ();
    spi_.send (buf, count);
    cs_.set ();
    if (command)
    {
        if (long_delay)
            delay_ms (3);
        else
            delay_us (60);
    }
}

} // namespace ucoo
