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
#include "gpio.host.hh"

#include <cstring>

namespace ucoo {

/// Shared context between instances.
class GpioShared
{
  public:
    /// Constructor, connect to mex node.
    GpioShared (Host &host);
    /// Register a new GPIO.
    void register_instance (Host &host, Gpio &instance);
    /// Set output state.
    void set (Gpio &instance, bool state);
    /// Set direction.
    void output (Gpio &instance, bool output);
  private:
    /// Send state change message.
    void send (Gpio &instance);
    /// Handle input value from Mex.
    void handle_gpio_input (mex::Msg &msg);
  private:
    Host &host_;
    mex::Node &node_;
    mex::mtype_t gpio_output_mtype_, gpio_input_mtype_;
    typedef std::map<std::string, Gpio *> Instances;
    Instances instances_;
};

GpioShared::GpioShared (Host &host)
    : host_ (host), node_ (host.get_node ())
{
    std::string instance = host_.get_instance ();
    gpio_output_mtype_ = node_.reserve (instance + ":gpio_out");
    gpio_input_mtype_ = node_.reserve (instance + ":gpio_in");
    node_.handler_register (gpio_input_mtype_, *this,
                            &GpioShared::handle_gpio_input);
}

void
GpioShared::register_instance (Host &host, Gpio &instance)
{
    assert (&host == &host_);
    std::pair<Instances::iterator, bool> r =
        instances_.insert (Instances::value_type (instance.name_, &instance));
    assert (r.second);
}

void
GpioShared::set (Gpio &instance, bool state)
{
    if (state != instance.output_)
    {
        instance.output_ = state;
        send (instance);
    }
}

void
GpioShared::output (Gpio &instance, bool output)
{
    if (output != instance.direction_output_)
    {
        instance.direction_output_ = output;
        send (instance);
    }
}

void
GpioShared::send (Gpio &instance)
{
    mex::Msg msg (gpio_output_mtype_);
    msg.push ("BB") << instance.direction_output_ << instance.output_;
    msg.push (instance.name_, std::strlen (instance.name_));
    node_.send (msg);
}

void
GpioShared::handle_gpio_input (mex::Msg &msg)
{
    int input, namelen;
    msg.pop ("B") >> input;
    namelen = msg.len ();
    std::string name (msg.pop (namelen), namelen);
    Instances::iterator i = instances_.find (name);
    assert (i != instances_.end ());
    i->second->input_ = input;
}

GpioShared *Gpio::shared_;

Gpio::Gpio (Host &host, const char *name)
    : name_ (name), input_ (false), output_ (false), direction_output_ (false)
{
    if (!shared_)
    {
        static GpioShared shared (host);
        shared_ = &shared;
    }
    shared_->register_instance (host, *this);
}

Gpio::Gpio ()
    : name_ (0), input_ (false), output_ (false), direction_output_ (false)
{
}

void
Gpio::set ()
{
    if (name_)
        shared_->set (*this, true);
}

void
Gpio::reset ()
{
    if (name_)
        shared_->set (*this, false);
}

void
Gpio::set (bool state)
{
    if (name_)
        shared_->set (*this, state);
}

void
Gpio::toggle ()
{
    if (name_)
        shared_->set (*this, !output_);
}

bool
Gpio::get () const
{
    return input_;
}

void
Gpio::input ()
{
    if (name_)
        shared_->output (*this, false);
}

void
Gpio::output ()
{
    if (name_)
        shared_->output (*this, true);
}

} // namespace ucoo
