#ifndef ucoo_dev_lcd_lcd_dummy_hh
#define ucoo_dev_lcd_lcd_dummy_hh
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
#include "ucoo/intf/lcd.hh"

namespace ucoo {

/// Dummy LCD, drop any message.
class LcdDummy : public Lcd
{
  public:
    /// Enable, initialise display.
    void enable ();
    /// Disable, clear display and shutdown if possible.
    void disable ();
    /// See Lcd::puts.
    void puts (const char *str);
    /// See Lcd::go.
    void go (int line, int column);
    /// See Lcd::clear.
    void clear ();
    /// See Lcd::get_lines.
    int get_lines () const;
    /// See Lcd::get_columns.
    int get_columns () const;
    /// See Lcd::printf.
    void printf (const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
};

} // namespace ucoo

#endif // ucoo_dev_lcd_lcd_dummy_hh
