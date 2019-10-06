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
#include "xmodem.hh"
#include "ucoolib/utils/delay.hh"
#include "ucoolib/common.hh"

namespace ucoo {

static const char xmodem_ack = 0x06;
static const char xmodem_c = 'C';
static const char xmodem_nack = 0x15;
static const char xmodem_eot = 0x04;
static const char xmodem_soh = 0x01;
static const char xmodem_stx = 0x02;
static const char xmodem_can = 0x18;

/// Get character with one second timeout.
static int
xmodem_getc (Stream &s)
{
    for (int timeout = 1000; timeout; timeout--)
    {
        int c = s.getc ();
        if (c != -1)
            return c;
        delay_ms (1);
    }
    return -1;
}

/// Blocking putc.
static void
xmodem_putc (Stream &s, int c)
{
    s.block (true);
    s.putc (c);
    s.block (false);
}

/// Update CRC on received payload.
static uint16_t
xmodem_crc_update (uint16_t crc, uint8_t c)
{
    int i;
    crc = crc ^ (c << 8);
    for (i = 0; i < 8; i++)
    {
        if (crc & 0x8000)
            crc = (crc << 1) ^ 0x1021;
        else
            crc = (crc << 1);
    }
    return crc;
}

int
xmodem_receive (Stream &s, XmodemReceiver &receiver)
{
    int i, length, total = 0;
    int c, c2;
    uint16_t crc;
    bool started = false;
    bool ok = false;
    uint8_t block_number = 1;
    bool dup = false;
    char buf[1024];
    s.block (false);
    while (1)
    {
        // Send ACK, NACK or 'C'.
        if (ok)
            xmodem_putc (s, xmodem_ack);
        else
        {
            // Purge.
            while (xmodem_getc (s) != -1)
                ;
            xmodem_putc (s, started ? xmodem_nack : xmodem_c);
        }
        ok = false;
        // Receive SOH, STX, or EOT.
        c = xmodem_getc (s);
        if (c == xmodem_soh)
            length = 128;
        else if (c == xmodem_stx)
            length = 1024;
        else if (c == xmodem_eot)
            break;
        else
            continue;
        // Receive block number.
        c = xmodem_getc (s);
        if (c == -1)
            continue;
        c2 = xmodem_getc (s);
        if (c2 == -1 || c2 != 255 - c)
            continue;
        else if (started && c == block_number - 1)
            dup = true;
        else if (c != block_number)
            continue;
        // Receive data.
        crc = 0;
        for (i = 0; i < length && c != -1; i++)
        {
            c = xmodem_getc (s);
            crc = xmodem_crc_update (crc, c);
            buf[i] = c;
        }
        if (c == -1)
            continue;
        // Receive CRC.
        c = xmodem_getc (s);
        if (c == -1)
            continue;
        crc = xmodem_crc_update (crc, c);
        c = xmodem_getc (s);
        if (c == -1)
            continue;
        crc = xmodem_crc_update (crc, c);
        if (crc != 0)
            continue;
        // Success, handle data.
        if (!dup)
        {
            started = true;
            block_number++;
            int r = receiver.write (buf, length);
            if (r == -1)
            {
                total = -1;
                break;
            }
            total += length;
        }
        ok = true;
        dup = false;
    }
    xmodem_putc (s, total == -1 ? xmodem_can : xmodem_ack);
    return total;
}

} // namespace ucoo
