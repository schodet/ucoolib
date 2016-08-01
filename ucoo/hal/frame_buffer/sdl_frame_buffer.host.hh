#ifndef ucoo_hal_sdl_frame_buffer_host_hh
#define ucoo_hal_sdl_frame_buffer_host_hh
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
#include <SDL.h>

namespace ucoo {

/// Simulated frame buffer using SDL.
class SdlFrameBuffer : public FrameBuffer
{
  public:
    SdlFrameBuffer (int w, int h)
        : FrameBuffer (w, h) { }
    void enable () override;
    void disable () override;
    void refresh () override;
    void fill (uint32_t color) override;
    void fill (uint32_t color, const Surface::Rect &rect) override;
    void blit (const Surface &surface, int x, int y) override;
    void blend (const Surface &surface, int x, int y,
                uint32_t const_color = 0xff000000) override;
  private:
    void sdl_error (const char *error);
  private:
    SDL_Window *win_;
    SDL_Renderer *rend_;
    SDL_Texture *tex_;
    uint32_t *pixels_;
};

} // namespace ucoo

#endif // ucoo_hal_sdl_frame_buffer_host_hh
