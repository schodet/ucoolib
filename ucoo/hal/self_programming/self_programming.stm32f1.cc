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
#include "ucoo/hal/self_programming/self_programming.hh"

#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/desig.h>
#include <libopencm3/stm32/dbgmcu.h>

namespace ucoo {

int
self_programming_flash_size ()
{
    return DESIG_FLASH_SIZE * 1024;
}

int
self_programming_erase_size (uint32_t addr)
{
    assert (static_cast<int> (addr - FLASH_BASE)
            <= self_programming_flash_size ());
    uint32_t idcode = DBGMCU_IDCODE & DBGMCU_IDCODE_DEV_ID_MASK;
    int page_size = idcode >= 0x414 ? 2048 : 1024;
    assert ((addr & (page_size - 1)) == 0);
    return page_size;
}

void
self_programming_erase (uint32_t addr, int count)
{
    assert (static_cast<int> (addr - FLASH_BASE + count)
            <= self_programming_flash_size ());
    int page_size = self_programming_erase_size (addr);
    assert ((count & (page_size - 1)) == 0);
    while (count)
    {
        flash_unlock ();
        flash_erase_page (addr);
        flash_lock ();
        addr += page_size;
        count -= page_size;
    }
    assert (count == 0);
}

void
self_programming_write (uint32_t addr, const char *buf, int count)
{
    assert (addr % 2 == 0);
    assert (reinterpret_cast<int> (buf) % 2 == 0);
    assert (count % 2 == 0);
    assert (static_cast<int> (addr - FLASH_BASE + count)
            <= self_programming_flash_size ());
    flash_unlock ();
    for (int i = 0; i < count; i += 2)
        flash_program_half_word (
            addr + i, *reinterpret_cast<const uint16_t *> (buf + i));
    flash_lock ();
}

} // namespace ucoo
