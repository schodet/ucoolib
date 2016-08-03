#ifndef ucoo_hal_frame_buffer_ltdc_stm32f4_hh
#define ucoo_hal_frame_buffer_ltdc_stm32f4_hh
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

/// LTDC controller.
class Ltdc
{
  public:
    /// Constructor, take timing parameters.
    Ltdc (int width, int heigth,
          int hsync, int hbp, int hfp,
          int vsync, int vbp, int vfp);
    /// Enable controller.
    void enable ();
    /// Disable controller.
    void disable ();
    /// Setup a layer.
    void layer_setup (int layer, const Surface &surface, int x, int y);
  protected:
    int width_, heigth_;
  private:
    int hsync_, hbp_, hfp_;
    int vsync_, vbp_, vfp_;
};

} // namespace ucoo

#endif // ucoo_hal_frame_buffer_ltdc_stm32f4_hh
