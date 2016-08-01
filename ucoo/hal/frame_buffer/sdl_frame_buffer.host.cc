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
#include "ucoo/hal/frame_buffer/sdl_frame_buffer.host.hh"

#include <iostream>
#include <cstdlib>

namespace ucoo {

void
SdlFrameBuffer::enable ()
{
    if (SDL_Init (SDL_INIT_VIDEO) != 0)
        sdl_error ("Unable to initialise SDL");
    atexit (SDL_Quit);
    win_ = SDL_CreateWindow ("GUI test", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    if (!win_)
        sdl_error ("Unable to create window");
    rend_ = SDL_CreateRenderer (win_, -1, 0);
    if (!rend_)
        sdl_error ("Unable to create renderer");
    tex_ = SDL_CreateTexture (rend_, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!tex_)
        sdl_error ("Unable to create texture");
    pixels_ = new uint32_t[width * height];
}

void
SdlFrameBuffer::disable ()
{
    SDL_Quit ();
}

void
SdlFrameBuffer::refresh ()
{
    SDL_UpdateTexture (tex_, nullptr, pixels_, width * 4);
    SDL_RenderClear (rend_);
    SDL_RenderCopy (rend_, tex_, nullptr, nullptr);
    SDL_RenderPresent (rend_);
}

void
SdlFrameBuffer::fill (uint32_t color)
{
    for (int i = 0; i < width * height; i++)
        pixels_[i] = color;
}

void
SdlFrameBuffer::fill (uint32_t color, const Surface::Rect &rect)
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
    if (x + w > width)
        w = width - x;
    if (y + h > height)
        h = height - y;
    if (w < 0 || h < 0)
        return;
    // Fill.
    uint32_t *p = pixels_ + y * width + x;
    for (; h; h--)
    {
        for (int iw = w; iw; iw--)
        {
            *p++ = color;
        }
        p += width - w;
    }
}

void
SdlFrameBuffer::blit (const Surface &surface, int x, int y)
{
    int sx = 0, sy = 0, sw = surface.w, sh = surface.h;
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
    if (x + sw > width)
        sw = width - x;
    if (y + sh > height)
        sh = height - y;
    if (sw < 0 || sh < 0)
        return;
    uint32_t *p = pixels_ + y * width + x;
    int line_skip = width - sw;
    // Blit.
    switch (surface.format)
    {
    case Surface::Format::ARGB8888:
        {
            const uint8_t *s = surface.pixels + (sy * surface.stride + sx) * 4;
            for (int j = sh; j; j--)
            {
                const uint32_t *ls = reinterpret_cast<const uint32_t *> (s);
                for (int i = sw; i; i--)
                    *p++ = *ls++;
                p += line_skip;
                s = s + surface.stride * 4;
            }
        }
        break;
    case Surface::Format::RGB888:
        {
            const uint8_t *s = surface.pixels + (sy * surface.stride + sx) * 3;
            for (int j = sh; j; j--)
            {
                const uint8_t *ls = s;
                for (int i = sw; i; i--)
                {
                    uint32_t w = *ls++;
                    w |= *ls++ << 8;
                    w |= *ls++ << 16;
                    w |= 0xff << 24;
                    *p++ = w;
                }
                p += line_skip;
                s = s + surface.stride * 3;
            }
        }
        break;
    case Surface::Format::RGB565:
        {
            const uint8_t *s = surface.pixels + (sy * surface.stride + sx) * 2;
            for (int j = sh; j; j--)
            {
                const uint16_t *ls = reinterpret_cast<const uint16_t *> (s);
                for (int i = sw; i; i--)
                {
                    uint16_t px = *ls++;
                    uint32_t r = px >> 11;
                    uint32_t g = (px >> 5) & 0x3f;
                    uint32_t b = px & 0x1f;
                    *p++ = 0xff << 24
                        | r << 19 | r >> 2 << 16
                        | g << 10 | g >> 4 << 8
                        | b << 3 | b >> 2;
                }
                p += line_skip;
                s = s + surface.stride * 2;
            }
        }
        break;
    case Surface::Format::ARGB4444:
        {
            const uint8_t *s = surface.pixels + (sy * surface.stride + sx) * 2;
            for (int j = sh; j; j--)
            {
                const uint16_t *ls = reinterpret_cast<const uint16_t *> (s);
                for (int i = sw; i; i--)
                {
                    uint16_t px = *ls++;
                    uint32_t r = (px >> 8) & 0xf;
                    uint32_t g = (px >> 4) & 0xf;
                    uint32_t b = px & 0xf;
                    *p++ = 0xff << 24
                        | r << 20 | r << 16
                        | g << 12 | g << 8
                        | b << 4 | b;
                }
                p += line_skip;
                s = s + surface.stride * 2;
            }
        }
        break;
    case Surface::Format::A8:
    case Surface::Format::A4:
        assert_unreachable ();
        break;
    }
}

void
SdlFrameBuffer::blend (const Surface &surface, int x, int y,
                       uint32_t const_color)
{
    int sx = 0, sy = 0, sw = surface.w, sh = surface.h;
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
    if (x + sw > width)
        sw = width - x;
    if (y + sh > height)
        sh = height - y;
    if (sw < 0 || sh < 0)
        return;
    uint32_t *p = pixels_ + y * width + x;
    int line_skip = width - sw;
    // Blend.
    uint32_t ca = (const_color >> 24) & 0xff;
    uint32_t cr = (const_color >> 16) & 0xff;
    uint32_t cg = (const_color >> 8) & 0xff;
    uint32_t cb = const_color & 0xff;
    switch (surface.format)
    {
    case Surface::Format::ARGB8888:
        {
            const uint8_t *s = surface.pixels + (sy * surface.stride + sx) * 4;
            for (int j = sh; j; j--)
            {
                const uint32_t *ls = reinterpret_cast<const uint32_t *> (s);
                for (int i = sw; i; i--)
                {
                    uint32_t spx = *ls++;
                    uint32_t a = spx >> 24;
                    a = a * ca / 255;
                    uint32_t sr = (spx >> 16) & 0xff;
                    uint32_t sg = (spx >> 8) & 0xff;
                    uint32_t sb = spx & 0xff;
                    uint32_t px = *p;
                    uint32_t r = (px >> 16) & 0xff;
                    uint32_t g = (px >> 8) & 0xff;
                    uint32_t b = px & 0xff;
                    r = (r * (255 - a) + sr * a) / 255;
                    g = (g * (255 - a) + sg * a) / 255;
                    b = (b * (255 - a) + sb * a) / 255;
                    *p++ = 0xff << 24 | r << 16 | g << 8 | b;
                }
                p += line_skip;
                s = s + surface.stride * 4;
            }
        }
        break;
    case Surface::Format::RGB888:
        {
            const uint8_t *s = surface.pixels + (sy * surface.stride + sx) * 3;
            for (int j = sh; j; j--)
            {
                const uint8_t *ls = s;
                for (int i = sw; i; i--)
                {
                    uint32_t sb = *ls++;
                    uint32_t sg = *ls++;
                    uint32_t sr = *ls++;
                    uint32_t px = *p;
                    uint32_t r = (px >> 16) & 0xff;
                    uint32_t g = (px >> 8) & 0xff;
                    uint32_t b = px & 0xff;
                    r = (r * (255 - ca) + sr * ca) / 255;
                    g = (g * (255 - ca) + sg * ca) / 255;
                    b = (b * (255 - ca) + sb * ca) / 255;
                    *p++ = 0xff << 24 | r << 16 | g << 8 | b;
                }
                p += line_skip;
                s = s + surface.stride * 3;
            }
        }
        break;
    case Surface::Format::RGB565:
        {
            const uint8_t *s = surface.pixels + (sy * surface.stride + sx) * 2;
            for (int j = sh; j; j--)
            {
                const uint16_t *ls = reinterpret_cast<const uint16_t *> (s);
                for (int i = sw; i; i--)
                {
                    uint16_t spx = *ls++;
                    uint32_t sr = spx >> 11;
                    sr = sr << 3 | sr >> 2;
                    uint32_t sg = (spx >> 5) & 0x3f;
                    sg = sg << 2 | sg >> 4;
                    uint32_t sb = spx & 0x1f;
                    sb = sb << 3 | sb >> 2;
                    uint32_t px = *p;
                    uint32_t r = (px >> 16) & 0xff;
                    uint32_t g = (px >> 8) & 0xff;
                    uint32_t b = px & 0xff;
                    r = (r * (255 - ca) + sr * ca) / 255;
                    g = (g * (255 - ca) + sg * ca) / 255;
                    b = (b * (255 - ca) + sb * ca) / 255;
                    *p++ = 0xff << 24 | r << 16 | g << 8 | b;
                }
                p += line_skip;
                s = s + surface.stride * 2;
            }
        }
        break;
    case Surface::Format::ARGB4444:
        {
            const uint8_t *s = surface.pixels + (sy * surface.stride + sx) * 2;
            for (int j = sh; j; j--)
            {
                const uint16_t *ls = reinterpret_cast<const uint16_t *> (s);
                for (int i = sw; i; i--)
                {
                    uint16_t spx = *ls++;
                    uint32_t a = spx >> 12;
                    a = a * ca / 255;
                    uint32_t sr = (spx >> 8) & 0xf;
                    sr = sr << 4 | sr;
                    uint32_t sg = (spx >> 4) & 0xf;
                    sg = sg << 4 | sg;
                    uint32_t sb = spx & 0xf;
                    sb = sb << 4 | sb;
                    uint32_t px = *p;
                    uint32_t r = (px >> 16) & 0xff;
                    uint32_t g = (px >> 8) & 0xff;
                    uint32_t b = px & 0xff;
                    r = (r * (15 - a) + sr * a) / 15;
                    g = (g * (15 - a) + sg * a) / 15;
                    b = (b * (15 - a) + sb * a) / 15;
                    *p++ = 0xff << 24 | r << 16 | g << 8 | b;
                }
                p += line_skip;
                s = s + surface.stride * 2;
            }
        }
        break;
    case Surface::Format::A8:
        {
            const uint8_t *s = surface.pixels + sy * surface.stride + sx;
            for (int j = sh; j; j--)
            {
                const uint8_t *ls = s;
                for (int i = sw; i; i--)
                {
                    uint32_t a = *ls++;
                    a = a * ca / 255;
                    uint32_t px = *p;
                    uint32_t r = (px >> 16) & 0xff;
                    uint32_t g = (px >> 8) & 0xff;
                    uint32_t b = px & 0xff;
                    r = (r * (255 - a) + cr * a) / 255;
                    g = (g * (255 - a) + cg * a) / 255;
                    b = (b * (255 - a) + cb * a) / 255;
                    *p++ = 0xff << 24 | r << 16 | g << 8 | b;
                }
                p += line_skip;
                s = s + surface.stride;
            }
        }
        break;
    case Surface::Format::A4:
        {
            assert (surface.stride % 2 == 0 && sx % 2 == 0 && sw % 2 == 0);
            const uint8_t *s = surface.pixels + (sy * surface.stride + sx) / 2;
            for (int j = sh; j; j--)
            {
                const uint8_t *ls = s;
                for (int i = sw / 2; i; i--)
                {
                    uint8_t aa = *ls++;
                    uint32_t a = (aa & 0xf);
                    a = a * ca / 255;
                    uint32_t px = *p;
                    uint32_t r = (px >> 16) & 0xff;
                    uint32_t g = (px >> 8) & 0xff;
                    uint32_t b = px & 0xff;
                    r = (r * (15 - a) + cr * a) / 15;
                    g = (g * (15 - a) + cg * a) / 15;
                    b = (b * (15 - a) + cb * a) / 15;
                    *p++ = 0xff << 24 | r << 16 | g << 8 | b;
                    a = aa >> 4;
                    a = a * ca / 255;
                    px = *p;
                    r = (px >> 16) & 0xff;
                    g = (px >> 8) & 0xff;
                    b = px & 0xff;
                    r = (r * (15 - a) + cr * a) / 15;
                    g = (g * (15 - a) + cg * a) / 15;
                    b = (b * (15 - a) + cb * a) / 15;
                    *p++ = 0xff << 24 | r << 16 | g << 8 | b;
                }
                p += line_skip;
                s = s + surface.stride / 2;
            }
        }
        break;
    }
}

void
SdlFrameBuffer::sdl_error (const char *error)
{
    std::cerr << error << ": " << SDL_GetError () << std::endl;
    exit (1);
}

} // namespace ucoo
