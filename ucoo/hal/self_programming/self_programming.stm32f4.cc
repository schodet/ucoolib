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
#include "ucoo/arch/reg.hh"

namespace ucoo {

static const uint32_t sector_addr[] = {
    // 16 KB
    0x8000000, 0x8004000, 0x8008000, 0x800c000,
    // 64 KB
    0x8010000,
    // 128 KB.
    0x8020000, 0x8040000, 0x8060000, 0x8080000, 0x80a0000, 0x80c0000,
    0x80e0000,
    // For two banks devices only:
    // 16 KB
    0x8100000, 0x8104000, 0x8108000, 0x810c000,
    // 64 KB
    0x8110000,
    // 128 KB.
    0x8120000, 0x8140000, 0x8160000, 0x8180000, 0x81a0000, 0x81c0000,
    0x81e0000, 0x8200000,
};

int
self_programming_flash_size ()
{
    return reg::DESIG->FLASH_SIZE * 1024;
}

int
self_programming_erase_size (uint32_t addr)
{
    for (int sector = 0; sector < lengthof (sector_addr) - 1; sector++)
    {
        if (addr == sector_addr[sector])
            return sector_addr[sector + 1] - addr;
    }
    assert_unreachable ();
}

void
self_programming_erase (uint32_t addr, int count)
{
    assert (static_cast<int> (addr - sector_addr[0] + count)
            <= self_programming_flash_size ());
    int sector;
    reg::FLASH->CR = FLASH_CR_LOCK;
    reg::FLASH->KEYR = FLASH_KEYR_KEY1;
    reg::FLASH->KEYR = FLASH_KEYR_KEY2;
    while (reg::FLASH->SR & FLASH_SR_BSY)
        ;
    reg::FLASH->CR = FLASH_CR_PSIZE_x32;
    for (sector = 0; count && sector < lengthof (sector_addr); sector++)
    {
        if (addr == sector_addr[sector])
        {
            int snb = sector >= 12 ? sector + 16 - 12 : sector;
            reg::FLASH->CR = FLASH_CR_PSIZE_x32 | (snb * FLASH_CR_SNB_0)
                | FLASH_CR_SER;
            reg::FLASH->CR |= FLASH_CR_STRT;
            while (reg::FLASH->SR & FLASH_SR_BSY)
                ;
            int sector_size = sector_addr[sector + 1] - addr;
            addr += sector_size;
            count -= sector_size;
        }
    }
    reg::FLASH->CR = FLASH_CR_LOCK;
    assert (count == 0);
}

void
self_programming_write (uint32_t addr, const char *buf, int count)
{
    assert (addr % 4 == 0);
    assert (reinterpret_cast<int> (buf) % 4 == 0);
    assert (count % 4 == 0);
    assert (static_cast<int> (addr - sector_addr[0] + count)
            <= self_programming_flash_size ());
    reg::FLASH->CR = FLASH_CR_LOCK;
    reg::FLASH->KEYR = FLASH_KEYR_KEY1;
    reg::FLASH->KEYR = FLASH_KEYR_KEY2;
    while (reg::FLASH->SR & FLASH_SR_BSY)
        ;
    reg::FLASH->CR = FLASH_CR_PSIZE_x32 | FLASH_CR_PG;
    for (int i = 0; i < count; i += 4)
    {
        *reinterpret_cast<volatile uint32_t *> (addr + i) =
            *reinterpret_cast<const uint32_t *> (buf + i);
	while (reg::FLASH->SR & FLASH_SR_BSY)
            ;
    }
    reg::FLASH->CR = FLASH_CR_LOCK;
}

} // namespace ucoo
