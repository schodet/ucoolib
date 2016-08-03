#ifndef ucoo_hal_frame_buffer_dma2d_stm32_hh
#define ucoo_hal_frame_buffer_dma2d_stm32_hh
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
#include "ucoo/intf/frame_buffer.hh"

namespace ucoo {

/// Wait until all operations are done.
void
dma2d_wait ();

/// Fill given rect in surface, with clipping.
void
dma2d_fill (Surface &dst, const Surface::Rect &rect, uint32_t color);

/// Blit surface in another surface, with clipping.
void
dma2d_blit (const Surface &dst, const Surface &src, int x, int y);

/// Blend surface in another surface, with clipping.
void
dma2d_blend (const Surface &dst, const Surface &src, int x, int y,
             uint32_t const_color = 0xff000000);

} // namespace ucoo

#endif // ucoo_hal_frame_buffer_dma2d_stm32_hh
