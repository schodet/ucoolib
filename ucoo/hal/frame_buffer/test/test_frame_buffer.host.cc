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
#include "ucoo/arch/arch.hh"
#include "ucoo/hal/frame_buffer/sdl_frame_buffer.host.hh"

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::SdlFrameBuffer fb (320, 320);
    fb.enable ();
    uint32_t buf[100][100];
    for (int y = 0; y < 100; y++)
        for (int x = 0; x < 100; x++)
            buf[x][y] = 0xff << 24
                | (x * 0xff / 99) << 16
                | (y * 0xff / 99) << 0;
    ucoo::Surface surf = { ucoo::Surface::Format::ARGB8888, 100, 100, 100,
        reinterpret_cast<uint8_t *> (buf) };
    uint8_t buf2[100][100];
    for (int y = 0; y < 100; y++)
        for (int x = 0; x < 100; x++)
            buf2[x][y] = (x + y) * 0xff / 198;
    ucoo::Surface surf2 = { ucoo::Surface::Format::A8, 100, 100, 100,
        reinterpret_cast<uint8_t *> (buf2) };
    uint16_t buf3[100][100];
    for (int y = 0; y < 100; y++)
        for (int x = 0; x < 100; x++)
            buf3[x][y] = 0x1f << 11
                | (x * 0x3f / 99) << 5
                | (y * 0x1f / 99) << 0;
    ucoo::Surface surf3 = { ucoo::Surface::Format::RGB565, 100, 100, 100,
        reinterpret_cast<uint8_t *> (buf3) };
    for (int i = -120; i < 320 + 120; i += 10)
    {
        fb.fill (0xffff0000);
        fb.fill (0xff00ff00, {i, 110, 100, 100});
        fb.blit (surf, 20, i);
        fb.blend (surf2, 120, i, 0xff0000ff);
        fb.blend (surf3, 220, i, 0x80000000);
        fb.refresh ();
        SDL_Delay (100);
    }
    return 0;
}
