// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2013 Nicolas Schodet
//
// APBTeam:
//        Web: http://apbteam.org/
//      Email: team AT apbteam DOT org
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
