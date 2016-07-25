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
#include "ucoo/hal/usb/usb_dwc_otg.stm32.hh"
#include "ucoo/hal/usb/usb_def.hh"

#include "ucoo/hal/gpio/gpio.hh"
#include "ucoo/arch/reg.hh"
#include "ucoo/arch/interrupt.arm.hh"
#include "ucoo/arch/rcc.stm32.hh"

namespace ucoo {

/// Information on USB hardware.
struct UsbHardware
{
    /// Base address.
    USB_OTG_TypeDef *base;
    /// RCC identifier.
    Rcc rcc;
    /// Corresponding IRQ.
    Irq irq;
    /// Used GPIO port 
    GpioPort &gpio_port;
#ifdef TARGET_stm32f4
    /// D+ and D- pins.
    int gpio_dp, gpio_dm;
    /// GPIO alternate function.
    int gpio_af;
#endif
};

static const UsbHardware usb_hardware[] =
{
    { reg::USB_OTG_FS, Rcc::OTGFS, Irq::OTG_FS, GPIOA,
#ifdef TARGET_stm32f4
        12, 11, 10
#endif
    },
#ifdef USB_OTG_HS_BASE
    { reg::USB_OTG_HS, Rcc::OTGHS, Irq::OTG_HS, GPIOB, 15, 14, 12 },
#endif
};

static UsbDriverDwcOtg *usb_instance;

template<>
void interrupt<Irq::OTG_FS> ()
{
    ucoo::assert (usb_instance);
    usb_instance->isr ();
}

#ifdef USB_OTG_HS_BASE
template<>
void interrupt<Irq::OTG_HS> ()
{
    ucoo::assert (usb_instance);
    usb_instance->isr ();
}
#endif

void
UsbDriverDwcOtg::enable ()
{
    assert (!usb_instance);
    usb_trace ("enable");
    enabled_ = true;
    usb_instance = this;
    const auto &hard = usb_hardware[static_cast<int> (inst_)];
    /// Setup GPIO (this is an exception of the global scheme where the user
    /// do this).
    hard.gpio_port.enable ();
#ifdef TARGET_stm32f4
    hard.gpio_port[hard.gpio_dp].af (hard.gpio_af);
    hard.gpio_port[hard.gpio_dm].af (hard.gpio_af);
#endif
    /// Enable clock.
    rcc_peripheral_clock_enable (hard.rcc);
    asm volatile ("nop"); // TODO: Do not know why this is needed.
    asm volatile ("nop");
    asm volatile ("nop");
    // Reset.
    while (!(hard.base->global.GRSTCTL & USB_OTG_GRSTCTL_AHBIDL))
        ;
    hard.base->global.GRSTCTL = USB_OTG_GRSTCTL_CSRST;
    while (hard.base->global.GRSTCTL & USB_OTG_GRSTCTL_CSRST)
        ;
    while (!(hard.base->global.GRSTCTL & USB_OTG_GRSTCTL_AHBIDL))
        ;
    // Setup USB core.
    assert (rcc_ahb_freq_hz > 32000000); // Or else, should change TRDT.
    hard.base->global.GUSBCFG = USB_OTG_GUSBCFG_FDMOD
        | (6 * USB_OTG_GUSBCFG_TRDT_0) | USB_OTG_GUSBCFG_PHYSEL;
    hard.base->global.GAHBCFG |= USB_OTG_GAHBCFG_GINT;
    hard.base->global.GINTMSK |= USB_OTG_GINTMSK_RXFLVLM
        | USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_USBRST
        | USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_IEPINT
        | USB_OTG_GINTMSK_SRQIM /* ? */
        | USB_OTG_GINTMSK_WUIM;
    // TODO: USB_OTG_GCCFG_VBDEN for F479.
    hard.base->global.GCCFG |= USB_OTG_GCCFG_VBUSBSEN | USB_OTG_GCCFG_PWRDWN;
    hard.base->device.DCFG |= USB_OTG_DCFG_NZLSOHSK | USB_OTG_DCFG_DSPD;
    hard.base->device.DCTL = 0;
    hard.base->device.DIEPMSK = USB_OTG_DIEPMSK_XFRCM;
    hard.base->device.DAINTMSK = USB_OTG_DAINTMSK_IEPM;
    // Enable interrupts.
    interrupt_enable (hard.irq);
}

void
UsbDriverDwcOtg::disable ()
{
    if (enabled_)
    {
        usb_trace ("disable");
        const auto &hard = usb_hardware[static_cast<int> (inst_)];
        hard.base->device.DCTL = USB_OTG_DCTL_SDIS;
        interrupt_disable (hard.irq);
        rcc_peripheral_clock_disable (hard.rcc);
#ifdef TARGET_stm32f4
        hard.gpio_port[hard.gpio_dp].input ();
        hard.gpio_port[hard.gpio_dm].input ();
#endif
        usb_instance = nullptr;
    }
}

int
UsbDriverDwcOtg::ep_write (uint8_t address, const char *data, int size)
{
    int ep = address & 0x7f;
    if (size > ep_in_size_[ep])
        size = ep_in_size_[ep];
    usb_trace ("write %d %d", ep, size);
    const auto &hard = usb_hardware[static_cast<int> (inst_)];
    assert (!(hard.base->ep_in[ep].DIEPCTL & USB_OTG_DIEPCTL_EPENA));
    hard.base->ep_in[ep].DIEPTSIZ = (1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos) | size;
    hard.base->ep_in[ep].DIEPCTL |=
        USB_OTG_DIEPCTL_EPENA | USB_OTG_DIEPCTL_CNAK;
    const uint32_t *data32 = reinterpret_cast<const uint32_t *> (data);
    for (int i = 0; i < size; i += 4)
        hard.base->fifo[ep].FIFO = *data32++;
    return size;
}

void
UsbDriverDwcOtg::ep_write_ready (uint8_t address)
{
    int ep = address & 0x7f;
    usb_trace ("write ready %d", ep);
    const auto &hard = usb_hardware[static_cast<int> (inst_)];
    irq_flags_t flags = irq_lock ();
    // Before the interrupt is locked, an interrupt could have been triggered.
    // This code might call ep_handle_in in this case, but only if no data was
    // written by the application.
    if (hard.base->ep_in[ep].DIEPCTL & USB_OTG_DIEPCTL_EPENA)
        // End point is enabled, will trigger an interrupt when done.
        ;
    else if (hard.base->ep_in[ep].DIEPINT & USB_OTG_DIEPINT_XFRC)
        // Interrupt triggered, waiting to deliver the interrupt.
        ;
    else
        // Restart data flow.
        ep_handle_in (address);
    irq_restore (flags);
}

int
UsbDriverDwcOtg::ep_read (uint8_t address, char *data, int size)
{
    int ep = address;
    assert (ep == rx_ep_);
    if (size > rx_length_)
        size = rx_length_;
    usb_trace ("read %d %d", ep, size);
    const auto &hard = usb_hardware[static_cast<int> (inst_)];
    uint32_t *data32 = reinterpret_cast<uint32_t *> (data);
    for (int i = 0; i < size / 4; i++)
        *data32++ = hard.base->fifo[0].FIFO;
    if (size % 4)
    {
        uint32_t w = hard.base->fifo[0].FIFO;
        uint8_t *data8 = reinterpret_cast<uint8_t *> (data32);
        for (int i = 0; i < size % 4; i++)
        {
            *data8++ = w & 0xff;
            w >>= 8;
        }
    }
    rx_length_ -= size;
    if (rx_length_ == 0)
        rx_ep_ = -1;
    return size;
}

void
UsbDriverDwcOtg::ep_read_ready (uint8_t address, int size)
{
    int ep = address;
    usb_trace ("read ready %d %d", ep, size);
    if (size >= ep_out_size_[ep])
    {
        const auto &hard = usb_hardware[static_cast<int> (inst_)];
        irq_flags_t flags = irq_lock ();
        if (!(hard.base->ep_out[ep].DOEPCTL & USB_OTG_DOEPCTL_EPENA))
        {
            hard.base->ep_out[ep].DOEPTSIZ = ep_out_doeptsiz_[ep];
            hard.base->ep_out[ep].DOEPCTL |= USB_OTG_DOEPCTL_EPENA
                | USB_OTG_DOEPCTL_CNAK;
            usb_trace ("read ready done");
        }
        irq_restore (flags);
    }
}

void
UsbDriverDwcOtg::ep_stall (uint8_t address, bool stall)
{
    assert (address == 0 && stall == true);
    usb_trace ("ep stall %d %d", address, stall);
    const auto &hard = usb_hardware[static_cast<int> (inst_)];
    hard.base->ep_in[0].DIEPCTL |= USB_OTG_DIEPCTL_STALL;
}

void
UsbDriverDwcOtg::set_address (uint8_t address)
{
    const auto &hard = usb_hardware[static_cast<int> (inst_)];
    hard.base->device.DCFG |= USB_OTG_DCFG_DAD_0 * address;
}

void
UsbDriverDwcOtg::ep_setup (uint8_t address, EpType type, int size)
{
    const auto &hard = usb_hardware[static_cast<int> (inst_)];
    if (address == 0)
    {
        usb_trace ("ep setup 0");
        assert (type == EpType::Control
                && size == USB_CONTROL_END_POINT_SIZE);
        hard.base->global.GRXFSIZ = rx_fifo_size_ / 4;
        fifo_used_ = rx_fifo_size_;
        hard.base->global.DIEPTXF0_HNPTXFSIZ =
            (USB_CONTROL_END_POINT_SIZE / 4) << 16 | fifo_used_ / 4;
        fifo_used_ += USB_CONTROL_END_POINT_SIZE;
        ep_in_size_[0] = USB_CONTROL_END_POINT_SIZE;
        ep_out_size_[0] = USB_CONTROL_END_POINT_SIZE;
        ep_out_doeptsiz_[0] = USB_OTG_DOEPTSIZ_STUPCNT
            | (1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos) | USB_CONTROL_END_POINT_SIZE;
        hard.base->ep_out[0].DOEPTSIZ = ep_out_doeptsiz_[0];
        hard.base->ep_out[0].DOEPCTL = USB_OTG_DOEPCTL_SNAK
            | USB_OTG_DOEPCTL_USBAEP;
        hard.base->ep_in[0].DIEPCTL = USB_OTG_DIEPCTL_SNAK
            | USB_OTG_DIEPCTL_USBAEP;
    }
    else
    {
        int ep = address & 0x7f;
        uint32_t epctl =
            USB_OTG_DOEPCTL_SD0PID_SEVNFRM
            | USB_OTG_DOEPCTL_SNAK
            | (type == EpType::Control ? 0 :
               type == EpType::Isochronous ? USB_OTG_DOEPCTL_EPTYP_0 :
               type == EpType::Bulk ? USB_OTG_DOEPCTL_EPTYP_1 :
               USB_OTG_DOEPCTL_EPTYP_1 | USB_OTG_DOEPCTL_EPTYP_0)
            | USB_OTG_DOEPCTL_USBAEP
            | size;
        if (address & 0x80)
        {
            usb_trace ("ep setup in %d %d %d", ep, static_cast<int> (type),
                       size);
            hard.base->global.DIEPTXF[ep - 1] = (size / 4) << 16
                | fifo_used_ / 4;
            fifo_used_ += size;
            ep_in_size_[ep] = size;
            hard.base->ep_in[ep].DIEPCTL = epctl
                | (ep * USB_OTG_DIEPCTL_TXFNUM_0);
        }
        else
        {
            usb_trace ("ep setup out %d %d %d", ep, static_cast<int> (type),
                       size);
            ep_out_size_[ep] = size;
            ep_out_doeptsiz_[ep] = (1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos) | size;
            hard.base->ep_out[ep].DOEPCTL = epctl;
        }
    }
}

void
UsbDriverDwcOtg::isr ()
{
    const auto &hard = usb_hardware[static_cast<int> (inst_)];
    uint32_t gintsts = hard.base->global.GINTSTS;
    if (gintsts & USB_OTG_GINTSTS_USBRST)
    {
        /* Reset. */
        usb_trace ("isr reset");
        hard.base->device.DCFG &= ~USB_OTG_DCFG_DAD;
        for (int i = 0; i < END_POINT_NB; i++)
            hard.base->ep_out[i].DOEPCTL = USB_OTG_DOEPCTL_SNAK;
        handle_reset ();
        hard.base->global.GINTSTS = USB_OTG_GINTSTS_USBRST;
    }
    if (gintsts & USB_OTG_GINTSTS_ENUMDNE)
    {
        /* Enumeration done. */
        usb_trace ("isr enumeration done");
        hard.base->global.GINTSTS = USB_OTG_GINTSTS_ENUMDNE;
    }
    if (gintsts & USB_OTG_GINTSTS_RXFLVL)
    {
        /* RX FIFO not empty. */
        isr_rx_fifo ();
    }
    if (gintsts & USB_OTG_GINTSTS_USBSUSP)
    {
        /* USB suspend. */
        usb_trace ("isr suspend");
        hard.base->global.GINTSTS = USB_OTG_GINTSTS_USBSUSP;
    }
    if (gintsts & USB_OTG_GINTSTS_IEPINT)
    {
        /* IN end point interrupt. */
        uint32_t daint = hard.base->device.DAINT;
        for (int i = 0; i < END_POINT_NB; i++)
        {
            if (daint & (1 << i))
            {
                usb_trace ("isr in xfrc %d", i);
                ep_handle_in (i ? 0x80 | i : 0);
                hard.base->ep_in[i].DIEPINT = USB_OTG_DIEPINT_XFRC;
            }
        }
    }
    if (gintsts & USB_OTG_GINTSTS_DISCINT)
    {
        /* Disconnect. */
        usb_trace ("isr disconnect");
        hard.base->global.GINTSTS = USB_OTG_GINTSTS_DISCINT;
    }
    if (gintsts & USB_OTG_GINTSTS_SRQINT)
    {
        /* New session. */
        usb_trace ("isr new session");
        hard.base->global.GINTSTS = USB_OTG_GINTSTS_SRQINT;
    }
    if (gintsts & USB_OTG_GINTSTS_WKUINT)
    {
        /* Wake up. */
        usb_trace ("isr wake up");
        hard.base->global.GINTSTS = USB_OTG_GINTSTS_WKUINT;
    }
}

void
UsbDriverDwcOtg::isr_rx_fifo ()
{
    const auto &hard = usb_hardware[static_cast<int> (inst_)];
    uint32_t rx = hard.base->global.GRXSTSP;
    uint32_t pkt_status = rx & USB_OTG_GRXSTSP_PKTSTS;
    int ep = rx & USB_OTG_GRXSTSP_EPNUM;
    int bcnt = (rx & USB_OTG_GRXSTSP_BCNT) >> USB_OTG_GRXSTSP_BCNT_Pos;
    if (pkt_status == USB_OTG_GRXSTSP_PKTSTS_Setup)
    {
        usb_trace ("isr rx setup");
        assert (ep == 0 && bcnt == 8);
        setup_buf_[0] = hard.base->fifo[0].FIFO;
        setup_buf_[1] = hard.base->fifo[0].FIFO;
    }
    else if (pkt_status == USB_OTG_GRXSTSP_PKTSTS_SetupCompleted)
    {
        usb_trace ("isr rx setup completed");
        handle_setup (&setup_buf_[0]);
    }
    else if (pkt_status == USB_OTG_GRXSTSP_PKTSTS_Out)
    {
        usb_trace ("isr rx out %d %d", ep, bcnt);
        rx_ep_ = ep;
        rx_length_ = bcnt;
        ep_handle_out (ep, bcnt);
        assert (rx_length_ == 0);
    }
    else if (pkt_status == USB_OTG_GRXSTSP_PKTSTS_OutCompleted)
        usb_trace ("isr rx out completed");
    else
        usb_trace ("isr rx other %d",
                   pkt_status >> USB_OTG_GRXSTSP_PKTSTS_Pos);
}

} // namespace ucoo
