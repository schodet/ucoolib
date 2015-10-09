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
#include "ucoo/base/fs/romfs/romfs.hh"
#include "ucoo/arch/arch.hh"
#include "ucoo/base/test/test.hh"

#include "test_fs.h"
#include <cstdio>

int
main (int argc, const char **argv)
{
    ucoo::arch_init (argc, argv);
    ucoo::test_stream_setup ();
    ucoo::RomFS romfs (test_fs, sizeof (test_fs));
    romfs.enable ();
    ucoo::Stream *s = romfs.open ("hello.txt");
    if (s)
    {
        while (1)
        {
            char buf[32];
            int r = s->read (buf, sizeof (buf));
            if (r <= 0)
            {
                if (r != -2)
                    printf ("\nread error\n");
                break;
            }
            else
                fwrite (buf, r, 1, stdout);
        }
        printf ("\ndone\n");
        romfs.close (s);
    }
    else
        printf ("open error\n");
    FILE *f = fopen ("hello.txt", "r");
    if (f)
    {
        while (1)
        {
            int c = fgetc (f);
            if (c == EOF)
                break;
            else
                putchar (c);
        }
        printf ("\ndone\n");
        fclose (f);
    }
    else
        printf ("fopen error\n");
}
