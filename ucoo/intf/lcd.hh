#ifndef ucoo_intf_lcd_hh
#define ucoo_intf_lcd_hh
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

namespace ucoo {

/// Character based LCD.
class Lcd
{
  public:
    /// Output a string.  Accept '\n' to go to next line and '\r' to return to
    /// start of line.
    virtual void puts (const char *str) = 0;
    /// Move cursor to given position.
    virtual void go (int line, int column) = 0;
    /// Clear the screen.
    virtual void clear () = 0;
    /// Get number of lines.
    virtual int get_lines () const = 0;
    /// Get number of columns.
    virtual int get_columns () const = 0;
    /// Helper for formated output.
    virtual void printf (const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
  protected:
    /// Default constructor.
    Lcd () { }
};

} // namespace ucoo

#endif // ucoo_intf_lcd_hh
