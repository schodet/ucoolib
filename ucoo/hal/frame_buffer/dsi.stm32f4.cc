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
#include "ucoo/hal/frame_buffer/dsi.stm32f4.hh"

#include "ucoo/arch/reg.hh"
#include "ucoo/arch/rcc.stm32.hh"

namespace ucoo {

enum DsiDataType
{
    SYNC_EVENT_VSYNC_START = 0x01,
    SYNC_EVENT_VSYNC_END = 0x11,
    SYNC_EVENT_HSYNC_START = 0x21,
    SYNC_EVENT_HSYNC_END = 0x31,
    END_OF_TRANSMISSION = 0x08,
    COLOR_MODE_OFF = 0x02,
    COLOR_MODE_ON = 0x12,
    NULL_PACKET = 0x09,
    BLANKING_PACKET = 0x19,
    GENERIC_SHORT_WRITE_PARAM_0 = 0x03,
    GENERIC_SHORT_WRITE_PARAM_1 = 0x13,
    GENERIC_SHORT_WRITE_PARAM_2 = 0x23,
    GENERIC_READ_PARAM_0 = 0x04,
    GENERIC_READ_PARAM_1 = 0x14,
    GENERIC_READ_PARAM_2 = 0x24,
    DCS_SHORT_WRITE_PARAM_0 = 0x05,
    DCS_SHORT_WRITE_PARAM_1 = 0x15,
    DCS_READ = 0x06,
    SET_MAX_RETURN_SIZE = 0x37,
    GENERIC_LONG_WRITE = 0x29,
    DCS_LONG_WRITE = 0x39,
    PACKED_PIXEL_RGB565 = 0x0e,
    PACKED_PIXEL_RGB666 = 0x1e,
    PACKED_PIXEL_RGB666_LOOSE = 0x2e,
    PACKED_PIXEL_RGB888 = 0x3e,
};

enum DcsCommand
{
    DCS_SET_TEAR_ON = 0x35,
};

bool Dsi::refreshing_;

Dsi::Dsi (int width, int heigth, int lanes)
    : Ltdc (width, heigth, 2, 1, 1, 2, 1, 1), lanes_ (lanes)
{
}

void
Dsi::enable (const Function<void ()> &config)
{
    // TODO: make this more configurable.
    // Enable clocks.
    rcc_peripheral_clock_enable (Rcc::DSI);
    // Enable DSI regulator.
    reg::DSI->WRPCR = DSI_WRPCR_REGEN;
    while (!(reg::DSI->WISR & DSI_WISR_RRS))
        ;
    // Enable PLL, 500 MHz output frequency (HSE @ 8 MHz).
    const int phy_hz = 500000000;
    reg::DSI->WRPCR = DSI_WRPCR_REGEN
        | (125 << DSI_WRPCR_PLL_NDIV_Pos)
        | (2 << DSI_WRPCR_PLL_IDF_Pos)
        | DSI_WRPCR_PLL_ODF_Div1;
    reg::DSI->WRPCR |= DSI_WRPCR_PLLEN;
    while (!(reg::DSI->WISR & DSI_WISR_PLLLS))
        ;
    //// D-PHY parameters.
    // Setup Unit interval.
    reg::DSI->WPCR[0] = 4000000000u / phy_hz;
    // Number of lanes.
    reg::DSI->PCONFR = lanes_ - 1;
    // Clock control.
    reg::DSI->CLCR = DSI_CLCR_DPCC;
    // TODO: Time for LP/HS and HS/LP transition in CLTCR/DLTCR.
    // TODO: Stop wait time in PCONFR.
    //// Timings.
    // TX escape clock division factor.
    reg::DSI->CCR = 4; // 500 MHz / 8 (bit) / 4 < 20 MHz
    // TX timeouts.
    reg::DSI->CCR |= 4 << 8;
    reg::DSI->TCCR[0] = 0x01000000;
    reg::DSI->TCCR[1] = 0x00000100;
    reg::DSI->TCCR[2] = 0x00000100;
    reg::DSI->TCCR[3] = 0x00000100;
    reg::DSI->TCCR[4] = 0x00000100;
    reg::DSI->TCCR[5] = 0x00000100;
    //// Flow control & DBI interface.
    reg::DSI->CMCR = DSI_CMCR_GSW0TX | DSI_CMCR_GSW1TX | DSI_CMCR_GSW2TX
        | DSI_CMCR_GSR0TX | DSI_CMCR_GSR1TX | DSI_CMCR_GSR2TX | DSI_CMCR_GLWTX
        | DSI_CMCR_DSW0TX | DSI_CMCR_DSW1TX | DSI_CMCR_DSR0TX | DSI_CMCR_DLWTX
        | DSI_CMCR_MRDPS;
    reg::DSI->GVCIDR = 0;
    //// LTDC interface.
    // Virtual channel for LTDC traffic.
    reg::DSI->LVCIDR = 0;
    // Color coding for the host and wrapper.
    reg::DSI->LCOLCR = DSI_LCOLCR_COLC_24Bit;
    reg::DSI->WCFGR = (reg::DSI->WCFGR & ~DSI_WCFGR_COLMUX)
        | DSI_WCFGR_COLMUX_24Bit;
    // Polarity of control signals.
    reg::DSI->LPCR = 0; // HSP, VSP, DEP
    reg::DSI->WCFGR |= 0; // VSPOL
    //// Configure adapted command mode.
    // Command mode.
    reg::DSI->MCR |= DSI_MCR_CMDM;
    reg::DSI->WCFGR |= DSI_WCFGR_DSIM;
    // Maximum memory write.
    reg::DSI->LCCR = width_;
    // Tearing effect source and polarity, no automatic refresh.
    reg::DSI->WCFGR &= ~(DSI_WCFGR_TESRC | DSI_WCFGR_AR);
    // Tearing effect acknowledge.
    reg::DSI->CMCR |= DSI_CMCR_TEARE;
    //// Enable D-PHY and D-PHY clock lane settings.
    reg::DSI->PCTLR = DSI_PCTLR_DEN;
    reg::DSI->PCTLR = DSI_PCTLR_CKE | DSI_PCTLR_DEN;
    //// Configure LTDC.
    rcc_sai_pll_setup (8000000,
                       4, // pllm => 8 MHz / 4 = 2 MHz
                       208, // plln => 2 MHz * 208 = 416 MHz
                       8, // pllp => 416 MHz / 8 = 52 MHz
                       15, 32, // pllq, pllq_div => 416 MHz / 15 / 32 ~= 0.86 MHz
                       5, 2); // pllr, pllr_div => 416 MHz / 5 / 2 ~= 41.6 MHz
    Ltdc::enable ();
    //// Enable DSI Host and DSI wrapper.
    reg::DSI->WIER = DSI_WIER_ERIE | DSI_WIER_TEIE;
    interrupt_enable (Irq::DSI);
    reg::DSI->CR = DSI_CR_EN;
    reg::DSI->WCR = DSI_WCR_DSIEN;
    //// Configure display.
    config ();
    //// Flow control & DBI interface.
    reg::DSI->CMCR &= ~(DSI_CMCR_GSW0TX | DSI_CMCR_GSW1TX | DSI_CMCR_GSW2TX
                        | DSI_CMCR_GSR0TX | DSI_CMCR_GSR1TX | DSI_CMCR_GSR2TX
                        | DSI_CMCR_GLWTX | DSI_CMCR_DSW0TX | DSI_CMCR_DSW1TX
                        | DSI_CMCR_DSR0TX | DSI_CMCR_DLWTX | DSI_CMCR_MRDPS);
    reg::DSI->PCR = DSI_PCR_BTAE;
}

void
Dsi::disable ()
{
    // TODO
    interrupt_disable (Irq::DSI);
    reg::DSI->CR = 0;
    reg::DSI->WCR = 0;
    Ltdc::disable ();
    rcc_peripheral_clock_disable (Rcc::DSI);
}

void
Dsi::layer_setup (int layer, const Surface &surface, int x, int y)
{
    reg::DSI->WCR &= ~DSI_WCR_DSIEN;
    Ltdc::layer_setup (layer, surface, x, y);
    reg::DSI->WCR |= DSI_WCR_DSIEN;
}

void
Dsi::refresh ()
{
    refreshing_ = true;
    ucoo::Dsi::write_command ({ DCS_SET_TEAR_ON, 0x00 });
    while (refreshing_)
        ucoo::barrier ();
}

void
Dsi::write_command (std::initializer_list<uint8_t> data)
{
    while (!(reg::DSI->GPSR & DSI_GPSR_CMDFE))
        ;
    uint32_t w = 0;
    int wb = 0;
    for (auto d : data)
    {
        w |= d << wb;
        wb += 8;
        if (wb == 32)
        {
            reg::DSI->GPDR = w;
            w = 0;
            wb = 0;
        }
    }
    if (data.size () == 1)
    {
        reg::DSI->GHCR = DCS_SHORT_WRITE_PARAM_0 * DSI_GHCR_DT0
            | 0 * DSI_GHCR_VCID0
            | w * DSI_GHCR_WCLSB0;
    }
    else if (data.size () == 2)
    {
        reg::DSI->GHCR = DCS_SHORT_WRITE_PARAM_1 * DSI_GHCR_DT0
            | 0 * DSI_GHCR_VCID0
            | w * DSI_GHCR_WCLSB0;
    }
    else
    {
        if (wb)
            reg::DSI->GPDR = w;
        reg::DSI->GHCR = DCS_LONG_WRITE * DSI_GHCR_DT0
            | 0 * DSI_GHCR_VCID0
            | data.size () * DSI_GHCR_WCLSB0;
    }
}

template<>
void
interrupt<Irq::DSI> ()
{
    if (reg::DSI->WISR & DSI_WISR_TEIF)
    {
        reg::DSI->WIFCR = DSI_WIFCR_CTEIF;
        ucoo::reg::DSI->WCR |= DSI_WCR_LTDCEN;
    }
    else if (reg::DSI->WISR & DSI_WISR_ERIF)
    {
        reg::DSI->WIFCR = DSI_WIFCR_CERIF;
        Dsi::refreshing_ = false;
    }
}

} // namespace ucoo
