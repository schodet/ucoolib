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
#include "ucoo/hal/frame_buffer/ltdc.stm32f4.hh"

#include "ucoo/arch/reg.hh"
#include "ucoo/arch/rcc.stm32.hh"

namespace ucoo {

Ltdc::Ltdc (int width, int heigth,
            int hsync, int hbp, int hfp,
            int vsync, int vbp, int vfp)
    : width_ (width), heigth_ (heigth),
      hsync_ (hsync), hbp_ (hbp), hfp_ (hfp),
      vsync_ (vsync), vbp_ (vbp), vfp_ (vfp)
{
}

void
Ltdc::enable ()
{
    // TODO: make this more configurable.
    // Enable clocks.
    rcc_peripheral_clock_enable (Rcc::LTDC);
    // Polarity of control signals.
    reg::LTDC->GCR &= (LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL
                       | LTDC_GCR_PCPOL);
    // Timings.
    reg::LTDC->SSCR = (hsync_ - 1) << 16 | (vsync_ - 1);
    reg::LTDC->BPCR = (hsync_ + hbp_ - 1) << 16 | (vsync_ + vbp_ - 1);
    reg::LTDC->AWCR = (hsync_ + hbp_ + width_ - 1) << 16
        | (vsync_ + vbp_ + heigth_ - 1);
    reg::LTDC->TWCR = (hsync_ + hbp_ + width_ + hfp_ - 1) << 16
        | (vsync_ + vbp_ + heigth_ + vfp_ - 1);
    // Enable.
    reg::LTDC->GCR |= LTDC_GCR_LTDCEN;
}

void
Ltdc::disable ()
{
    reg::LTDC->GCR &= ~LTDC_GCR_LTDCEN;
    rcc_peripheral_clock_disable (Rcc::LTDC);
    // TODO: shut down PLL.
}

void
Ltdc::layer_setup (int layer, const Surface &surface, int x, int y)
{
    auto reg = layer == 0 ? reg::LTDC_Layer1 : reg::LTDC_Layer2;
    reg->WHPCR = (hsync_ + hbp_ + x + surface.w - 1) << 16
        | (hsync_ + hbp_ + x);
    reg->WVPCR = (vsync_ + vbp_ + y + surface.h - 1) << 16
        | (vsync_ + vbp_ + y);
    reg->PFCR = static_cast<int> (surface.format) & 0xff;
    reg->CFBAR = reinterpret_cast<uintptr_t> (surface.pixels);
    reg->CFBLR = (surface.stride * surface.bpp () / 8) << 16
        | (surface.w * surface.bpp () / 8 + 3);
    reg->CFBLNR = surface.h;
    reg->CR = LTDC_LxCR_LEN;
    // Load parameters.
    reg::LTDC->SRCR = LTDC_SRCR_IMR;
}

} // namespace ucoo
