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
#include "usdist.hh"

namespace ucoo {


UsDist::UsDist (UsDistControl &ctrl, Adc &adc, Io &io,
                int distance_min, int distance_max, int distance_too_far,
                int resolution)
    : adc_ (adc), io_ (io), next_ (0),
      distance_min_ (distance_min), distance_max_ (distance_max),
      distance_too_far_ (distance_too_far), resolution_ (resolution),
      distance_ (-1)
{
    next_ = ctrl.sensors_;
    ctrl.sensors_ = this;
    if (resolution_ == -1)
        resolution_ = adc_.get_resolution ();
}

void
UsDist::enable ()
{
    io_.set ();
}

void
UsDist::disable ()
{
    io_.reset ();
}

int
UsDist::read ()
{
    int v = adc_.read ();
    // The sensor returns a value between 4 and 20 mA proportional to the
    // distance between calibrated values.  Ignore faulty sensors.
    const int max = resolution_;
    const int min = max / 5;
    if (v <= min / 4)
        distance_ = -1;
    else if (v <= min)
        distance_ = distance_min_;
    else
        distance_ = distance_min_
            + ((v - min) * (distance_max_ - distance_min_) / (max - min));
    if (distance_too_far_ != -1 && distance_ > distance_too_far_)
        distance_ = -1;
    return distance_;
}

UsDistControl::UsDistControl (int period)
    : period_ (period), sensors_ (0), current_ (0), wait_ (0)
{
}

bool
UsDistControl::update ()
{
    bool new_measure = false;
    if (wait_)
        wait_--;
    else
    {
        if (current_)
        {
            current_->disable ();
            current_->read ();
            new_measure = true;
            current_ = current_->next_;
        }
        if (!current_)
            current_ = sensors_;
        current_->enable ();
        wait_ = period_ - 1;
    }
    return new_measure;
}

} // namespace ucoo
