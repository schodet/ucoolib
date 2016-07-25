#ifndef ucoo_hal_gpio_gpio_stm32f4_hh
#define ucoo_hal_gpio_gpio_stm32f4_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
#include "ucoo/intf/io.hh"
#include "ucoo/arch/reg.hh"
#include "ucoo/arch/rcc.stm32.hh"

namespace ucoo {

class GpioPort;

/// General purpose input/output on STM32F4.
class Gpio : public Io
{
  public:
    enum class Pull : uint32_t
    {
        NONE,
        UP,
        DOWN,
    };
    enum class Speed : uint32_t
    {
        SPEED_2MHZ,
        SPEED_25MHZ,
        SPEED_50MHZ,
        SPEED_100MHZ,
    };
    enum class Type : uint32_t
    {
        PUSH_PULL,
        OPEN_DRAIN,
    };
  public:
    /// See Io::set.
    void set ();
    /// See Io::reset.
    void reset ();
    /// See Io::set.
    void set (bool state);
    /// See Io::toggle.
    void toggle ();
    /// See Io::get.
    bool get () const;
    /// See Io::input.
    void input ();
    /// See Io::output.
    void output ();
    /// Set pull-up or pull-down.
    void pull (Pull dir);
    /// Set output speed.
    void speed (Speed s);
    /// Set output type.
    void type (Type t);
    /// Set alternate function.
    void af (int num);
    /// Set as analog.
    void analog ();
  private:
    /// Constructor, take a port, and pin BIT number.
    Gpio (GpioPort &port, int bit);
    friend GpioPort;
  private:
    /// Port register base address.
    GPIO_TypeDef * const port_;
    /// IO bitmask.
    const uint16_t mask_;
};

/// General purpose input/output port on STM32F4.
class GpioPort
{
    GPIO_TypeDef * const port_;
    const Rcc rcc_;
    friend class Gpio;
  public:
    /// Constructor.
    GpioPort (GPIO_TypeDef *port, Rcc rcc)
        : port_ (port), rcc_ (rcc) { }
    /// Enable port.
    void enable ()
        { rcc_peripheral_clock_enable (rcc_); }
    /// Disable port.
    void disable ()
        { rcc_peripheral_clock_disable (rcc_); }
    /// Return a port GPIO.
    Gpio operator[] (int bit)
        { return Gpio (*this, bit); }
    /// Get the index of the port, used with Exti.
    int get_port_index () const;
};

inline
Gpio::Gpio (GpioPort &port, int bit)
    : port_ (port.port_), mask_ (1u << bit)
{
}

extern GpioPort GPIOA;
extern GpioPort GPIOB;
extern GpioPort GPIOC;
extern GpioPort GPIOD;
extern GpioPort GPIOE;
extern GpioPort GPIOF;
extern GpioPort GPIOG;
extern GpioPort GPIOH;
extern GpioPort GPIOI;
extern GpioPort GPIOJ;
extern GpioPort GPIOK;

} // namespace ucoo

#endif // ucoo_hal_gpio_gpio_stm32f4_hh
