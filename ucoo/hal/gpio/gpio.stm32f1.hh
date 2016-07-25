#ifndef ucoo_hal_gpio_gpio_stm32f1_hh
#define ucoo_hal_gpio_gpio_stm32f1_hh
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
#include "ucoo/intf/io.hh"
#include "ucoo/arch/reg.hh"
#include "ucoo/arch/rcc.stm32.hh"

namespace ucoo {

class GpioPort;

/// General purpose input/output on STM32F1.
class Gpio : public Io
{
  public:
    enum class InputCnf : uint8_t
    {
        ANALOG = GPIO_CNF_Input_Analog << 2,
        FLOAT = GPIO_CNF_Input_Float << 2,
        // Up/Down is selected using output register.
        PULL_UPDOWN = GPIO_CNF_Input_PullUpDown << 2,
    };
    enum class OutputCnf : uint8_t
    {
        PUSHPULL = GPIO_CNF_Output_PushPull << 2,
        OPENDRAIN = GPIO_CNF_Output_OpenDrain << 2,
        AF_PUSH_PULL = GPIO_CNF_Output_AFPushPull << 2,
        AF_OPENDRAIN = GPIO_CNF_Output_AFOpenDrain << 2,
    };
    enum class Speed : uint8_t
    {
        SPEED_2MHZ = GPIO_MODE_Output_2MHz,
        SPEED_10MHZ = GPIO_MODE_Output_10MHz,
        SPEED_50MHZ = GPIO_MODE_Output_50MHz,
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
    /// Set input configuration, used when port is input.
    void input_cnf (InputCnf cnf);
    /// Set output configuration, used when port is output.
    void output_cnf (OutputCnf cnf);
    /// Set output speed.
    void speed (Speed s);
  private:
    /// Constructor, take a port, and pin BIT number.
    Gpio (GpioPort &port, int bit);
    friend GpioPort;
  private:
    /// Port register base address.
    GPIO_TypeDef * const port_;
    /// IO bitmask.
    const uint16_t mask_;
    /// Configuration for input.
    InputCnf input_cnf_;
    /// Configuration for output.
    OutputCnf output_cnf_;
    /// Speed for output.
    Speed speed_;
    /// Current intput/output state.
    bool output_;
};

/// General purpose input/output port on STM32F1.
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
    : port_ (port.port_), mask_ (1u << bit),
      input_cnf_ (InputCnf::FLOAT),
      output_cnf_ (OutputCnf::PUSHPULL),
      speed_ (Speed::SPEED_10MHZ),
      output_ (false)
{
}

extern GpioPort GPIOA;
extern GpioPort GPIOB;
extern GpioPort GPIOC;
extern GpioPort GPIOD;
extern GpioPort GPIOE;

} // namespace ucoo

#endif // ucoo_hal_gpio_gpio_stm32f1_hh
