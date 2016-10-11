#ifndef ucoo_intf_frame_buffer_hh
#define ucoo_intf_frame_buffer_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2016 Nicolas Schodet
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
#include "ucoo/common.hh"

namespace ucoo {

namespace details {

constexpr int
format_enum (int bpp, int order)
{
    return bpp << 8 | order;
}

} // namespace details

/// Surface (picture with its information to blit in frame buffer).
struct Surface
{
    /// Stored pixels format.
    enum class Format
    {
        /// 32 bit, Ax8, Rx8, Gx8, Bx8.
        ARGB8888 = details::format_enum (32, 0),
        /// 24 bit, Rx8, Gx8, Bx8.
        RGB888 = details::format_enum (24, 1),
        /// 16 bit, Rx5, Gx6, Bx5.
        RGB565 = details::format_enum (16, 2),
        /// 16 bit, Ax4, Rx4, Gx4, Bx4.
        ARGB4444 = details::format_enum (16, 4),
        /// 8 bit, Ax8.
        A8 = details::format_enum (8, 9),
        /// 4 bit, Ax4.
        A4 = details::format_enum (4, 10),
    };
    Format format;
    /// Size.
    int w, h;
    /// Offset between begin of line and begin of next line in pixels.
    int stride;
    /// Pointer to pixel data.
    uint8_t *pixels;
  public:
    /// Return the number of bytes per pixels for a given format.
    static constexpr int
    format_bpp (Format format) { return static_cast<int> (format) >> 8; }
    /// Return the number of bytes per pixels.
    int bpp () const { return format_bpp (format); }
    /// Rectangle used to define a region in surface.
    struct Rect
    {
        /// Top left coordinates.
        int x, y;
        /// Width and height.
        int w, h;
    };
};

/// Double buffered frame buffer interface.
class FrameBuffer
{
  public:
    /// Enable.
    virtual void enable () = 0;
    /// Disable.
    virtual void disable () = 0;
    /// Turn display on or off.
    virtual void on (bool state = true) = 0;
    /// Show hidden buffer on screen.
    virtual void refresh () = 0;
    /// Fill with color.
    virtual void fill (uint32_t color) = 0;
    /// Fill rectangle with color.
    virtual void fill (uint32_t color, const Surface::Rect &rect) = 0;
    /// Blit a surface.
    virtual void blit (const Surface &surface, int x, int y) = 0;
    /// Blit a surface with blending.
    virtual void blend (const Surface &surface, int x, int y,
                        uint32_t const_color = 0xff000000) = 0;
  protected:
    /// Constructor.
    FrameBuffer (int w, int h)
        : width (w), height (h) { }
  public:
    /// Screen size.
    int width, height;
};

} // namespace ucoo

#endif // ucoo_intf_frame_buffer_hh
