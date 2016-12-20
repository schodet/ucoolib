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

Dsi::Dsi (int width, int heigth, int lanes, bool video_mode,
          int hsync, int hbp, int hfp,
          int vsync, int vbp, int vfp)
    : Ltdc (width, heigth, hsync, hbp, hfp, vsync, vbp, vfp), lanes_ (lanes),
      video_mode_ (video_mode)
{
}

void
Dsi::enable (const Function<void ()> &config, int phy_hz, int pclk_hz)
{
    // Enable clocks.
    rcc_peripheral_clock_enable (Rcc::DSI);
    // Enable DSI regulator.
    reg::DSI->WRPCR = DSI_WRPCR_REGEN;
    while (!(reg::DSI->WISR & DSI_WISR_RRS))
        ;
    // Enable PLL.
    int ndiv = phy_hz * 2 / rcc_hse_freq_hz;
    assert (ndiv >= 10 && ndiv <= 125
            && rcc_hse_freq_hz / 2 * ndiv == phy_hz);
    reg::DSI->WRPCR = DSI_WRPCR_REGEN
        | (ndiv << DSI_WRPCR_PLL_NDIV_Pos)
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
    // Time for LP/HS and HS/LP transition (using workaround for clock) &
    // maximum read time.
    reg::DSI->CLTCR = 35 * DSI_CLTCR_HS2LP_TIME0 | 35 * DSI_CLTCR_LP2HS_TIME0;
    reg::DSI->DLTCR = 35 * DSI_DLTCR_HS2LP_TIME0 | 35 * DSI_DLTCR_LP2HS_TIME0
        | 256 * DSI_DLTCR_MRD_TIME0;
    // Stop wait time.
    reg::DSI->PCONFR |= 10 * DSI_PCONFR_SW_TIME0;
    //// Timings.
    // TX escape clock division factor.
    int txeckdiv = (phy_hz + (8 * 20000000 - 1)) / (8 * 20000000);
    assert (txeckdiv >= 2 && txeckdiv <= 255);
    reg::DSI->CCR = txeckdiv;
    // TX timeouts.
    reg::DSI->TCCR[0] = 0; // No HS or LP TX timeout.
    reg::DSI->TCCR[1] = 256; // HS read.
    reg::DSI->TCCR[2] = 256; // LP read.
    reg::DSI->TCCR[3] = 256; // HS write.
    reg::DSI->TCCR[4] = 256; // LP write.
    reg::DSI->TCCR[5] = 256; // BTA.
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
    if (!video_mode_)
    {
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
        reg::DSI->PCR |= DSI_PCR_BTAE;
    }
    else
    {
        //// Configure video mode.
        reg::DSI->VMCR = DSI_VMCR_VMT_Burst | DSI_VMCR_LPVSAE
            | DSI_VMCR_LPVBPE | DSI_VMCR_LPVFPE | DSI_VMCR_LPVAE
            | DSI_VMCR_LPHBPE | DSI_VMCR_LPHFPE | DSI_VMCR_LPCE;
        reg::DSI->MCR &= ~DSI_MCR_CMDM;
        reg::DSI->WCFGR &= ~DSI_WCFGR_DSIM;
        // Packet size.
        reg::DSI->VPCR = width_;
        reg::DSI->VCCR = 0;
        // Timings.
        reg::DSI->VHSACR = static_cast<long long> (hsync_)
            * (phy_hz / 8) / pclk_hz;
        reg::DSI->VHBPCR = static_cast<long long> (hbp_)
            * (phy_hz / 8) / pclk_hz;
        reg::DSI->VLCR = static_cast<long long> (hsync_ + hbp_ + width_ + hfp_)
            * (phy_hz / 8) / pclk_hz;
        reg::DSI->VVSACR = vsync_;
        reg::DSI->VVBPCR = vbp_;
        reg::DSI->VVFPCR = vfp_;
        reg::DSI->VVACR = heigth_;
        reg::DSI->LPMCR = 64 * DSI_LPMCR_LPSIZE0 | 0 * DSI_LPMCR_VLPSIZE0;
    }
    //// Enable D-PHY and D-PHY clock lane settings.
    reg::DSI->PCTLR = DSI_PCTLR_DEN;
    reg::DSI->PCTLR = DSI_PCTLR_CKE | DSI_PCTLR_DEN;
    //// Configure LTDC.
    Ltdc::enable ();
    //// Enable DSI Host and DSI wrapper.
    reg::DSI->WIER = DSI_WIER_ERIE | DSI_WIER_TEIE;
    interrupt_enable (Irq::DSI);
    reg::DSI->CR = DSI_CR_EN;
    reg::DSI->WCR = DSI_WCR_DSIEN;
    //// Configure display.
    config ();
    if (!video_mode_)
    {
        // Need HS to transmit pixels.
        reg::DSI->CMCR &= ~(DSI_CMCR_GSW0TX | DSI_CMCR_GSW1TX | DSI_CMCR_GSW2TX
                            | DSI_CMCR_GSR0TX | DSI_CMCR_GSR1TX | DSI_CMCR_GSR2TX
                            | DSI_CMCR_GLWTX | DSI_CMCR_DSW0TX | DSI_CMCR_DSW1TX
                            | DSI_CMCR_DSR0TX | DSI_CMCR_DLWTX | DSI_CMCR_MRDPS);
    }
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
    if (!video_mode_)
    {
        reg::DSI->WCR &= ~DSI_WCR_DSIEN;
        Ltdc::layer_setup (layer, surface, x, y);
        reg::DSI->WCR |= DSI_WCR_DSIEN;
    }
    else
        Ltdc::layer_setup (layer, surface, x, y);
}

void
Dsi::refresh (bool wait_hsync)
{
    if (!video_mode_)
    {
        refreshing_ = true;
        if (wait_hsync)
            ucoo::Dsi::write_command ({ DCS_SET_TEAR_ON, 0x00 });
        else
            ucoo::reg::DSI->WCR |= DSI_WCR_LTDCEN;
        while (refreshing_)
            ucoo::barrier ();
    }
}

void
Dsi::write_command (std::initializer_list<uint8_t> data)
{
    while (!(reg::DSI->GPSR & DSI_GPSR_CMDFE))
        ;
    while (!(reg::DSI->GPSR & DSI_GPSR_PWRFE))
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
