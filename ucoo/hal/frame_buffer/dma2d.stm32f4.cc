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
#include "ucoo/hal/frame_buffer/dma2d.stm32f4.hh"
#include "ucoo/arch/reg.hh"

namespace ucoo {

void
dma2d_wait ()
{
    while (reg::DMA2D->CR & DMA2D_CR_START)
        ;
}

void
dma2d_fill (Surface &dst, const Surface::Rect &rect, uint32_t color)
{
    // Handle clipping.
    int x = rect.x, y = rect.y, w = rect.w, h = rect.h;
    if (x < 0)
    {
        w += x;
        x = 0;
    }
    if (y < 0)
    {
        h += y;
        y = 0;
    }
    if (x + w > dst.w)
        w = dst.w - x;
    if (y + h > dst.h)
        h = dst.h - y;
    if (w < 0 || h < 0)
        return;
    // Wait for DMA2D to be idle.
    dma2d_wait ();
    // Fill.
    switch (dst.format)
    {
    case Surface::Format::ARGB8888:
    case Surface::Format::RGB888:
        reg::DMA2D->OCOLR = color;
        break;
    case Surface::Format::RGB565:
        {
            uint32_t r = (color >> 19) & 0x1f;
            uint32_t g = (color >> 10) & 0x3f;
            uint32_t b = (color >> 3) & 0x1f;
            reg::DMA2D->OCOLR = r << 11 | g << 5 | b;
        }
        break;
    case Surface::Format::ARGB4444:
        {
            uint32_t r = (color >> 20) & 0xf;
            uint32_t g = (color >> 12) & 0xf;
            uint32_t b = (color >> 4) & 0xf;
            reg::DMA2D->OCOLR = r << 8 | g << 4 | b;
        }
        break;
    case Surface::Format::A8:
    case Surface::Format::A4:
        assert_unreachable ();
    }
    int bpp = dst.bpp ();
    reg::DMA2D->OPFCCR = static_cast<int> (dst.format) & 0xff;
    reg::DMA2D->OMAR = reinterpret_cast<intptr_t> (dst.pixels)
        + (y * dst.stride + x) * bpp / 8;
    reg::DMA2D->OOR = dst.stride - w;
    reg::DMA2D->NLR = w << 16 | h;
    reg::DMA2D->CR = DMA2D_CR_MODE_RegisterToMemory | DMA2D_CR_START;
}

void
dma2d_blit (const Surface &dst, const Surface &src, int x, int y)
{
    int sx = 0, sy = 0, sw = src.w, sh = src.h;
    // Handle clipping.
    if (x < 0)
    {
        sw += x;
        sx = -x;
        x = 0;
    }
    if (y < 0)
    {
        sh += y;
        sy = -y;
        y = 0;
    }
    if (x + sw > dst.w)
        sw = dst.w - x;
    if (y + sh > dst.h)
        sh = dst.h - y;
    if (sw < 0 || sh < 0)
        return;
    // Wait for DMA2D to be idle.
    dma2d_wait ();
    // Blit.
    int srcbpp = src.bpp ();
    int dstbpp = dst.bpp ();
    reg::DMA2D->FGMAR = reinterpret_cast<intptr_t> (src.pixels)
        + (sy * src.stride + sx) * srcbpp / 8;
    reg::DMA2D->FGOR = src.stride - sw;
    reg::DMA2D->FGPFCCR = static_cast<int> (src.format) & 0xff;
    reg::DMA2D->OPFCCR = static_cast<int> (dst.format) & 0xff;
    reg::DMA2D->OMAR = reinterpret_cast<intptr_t> (dst.pixels)
        + (y * dst.stride + x) * dstbpp / 8;
    reg::DMA2D->OOR = dst.stride - sw;
    reg::DMA2D->NLR = sw << 16 | sh;
    if (src.format == dst.format)
        reg::DMA2D->CR = DMA2D_CR_MODE_MemoryToMemory | DMA2D_CR_START;
    else
        reg::DMA2D->CR = DMA2D_CR_MODE_MemoryToMemoryFPC | DMA2D_CR_START;
}

void
dma2d_blend (const Surface &dst, const Surface &src, int x, int y,
             uint32_t const_color)
{
    int sx = 0, sy = 0, sw = src.w, sh = src.h;
    // Handle clipping.
    if (x < 0)
    {
        sw += x;
        sx = -x;
        x = 0;
    }
    if (y < 0)
    {
        sh += y;
        sy = -y;
        y = 0;
    }
    if (x + sw > dst.w)
        sw = dst.w - x;
    if (y + sh > dst.h)
        sh = dst.h - y;
    if (sw < 0 || sh < 0)
        return;
    // Wait for DMA2D to be idle.
    dma2d_wait ();
    // Blend.
    int srcbpp = src.bpp ();
    int dstbpp = dst.bpp ();
    reg::DMA2D->FGMAR = reinterpret_cast<intptr_t> (src.pixels)
        + (sy * src.stride + sx) * srcbpp / 8;
    reg::DMA2D->FGOR = src.stride - sw;
    uint32_t bgmar = reinterpret_cast<intptr_t> (dst.pixels)
        + (y * dst.stride + x) * dstbpp / 8;
    reg::DMA2D->BGMAR = bgmar;
    uint32_t bgor = dst.stride - sw;
    reg::DMA2D->BGOR = bgor;
    reg::DMA2D->FGPFCCR = (const_color & 0xff000000)
        | DMA2D_FGPFCCR_AM_1
        | (static_cast<int> (src.format) & 0xff);
    reg::DMA2D->FGCOLR = const_color & 0xffffff;
    reg::DMA2D->BGPFCCR = static_cast<int> (dst.format) & 0xff;
    reg::DMA2D->OPFCCR = static_cast<int> (dst.format) & 0xff;
    reg::DMA2D->OMAR = bgmar;
    reg::DMA2D->OOR = bgor;
    reg::DMA2D->NLR = sw << 16 | sh;
    reg::DMA2D->CR = DMA2D_CR_MODE_MemoryToMemoryBlend | DMA2D_CR_START;
}

} // namespace ucoo
