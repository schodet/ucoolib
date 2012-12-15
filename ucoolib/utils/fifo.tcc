#ifndef ucoolib_utils_fifo_tcc
#define ucoolib_utils_fifo_tcc
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2012 Nicolas Schodet
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
#include <algorithm>

namespace ucoo {

template<typename T, int size>
inline
Fifo<T, size>::Fifo ()
    : head_ (0), tail_ (0)
{
}

template<typename T, int size>
inline T
Fifo<T, size>::pop ()
{
    // Reader, can only update head.
    int head = head_;
    assert (head != tail_);
    T v = buffer_[head];
    head = next (head);
    barrier ();
    head_ = head;
    return v;
}

template<typename T, int size>
inline void
Fifo<T, size>::push (const T &e)
{
    // Writer, can only update tail.
    int tail = tail_;
    buffer_[tail] = e;
    tail = next (tail);
    assert (head_ != tail);
    barrier ();
    tail_ = tail;
}

template<typename T, int size>
inline int
Fifo<T, size>::read (T *buf, int count)
{
    // Reader, can only update head.
    int r = 0;
    int head = head_;
    int tail = access_once (tail_);
    while (r < count && head != tail)
    {
        buf[r] = buffer_[head];
        head = next (head);
        r++;
    }
    // Ensure data is copied, then update head.
    barrier ();
    head_ = head;
    return r;
}

template<typename T, int size>
inline int
Fifo<T, size>::write (const T *buf, int count)
{
    // Writer, can only update tail.
    int r = 0;
    int head = access_once (head_);
    int tail = tail_;
    int tailp1 = next (tail);
    while (r < count && tailp1 != head)
    {
        buffer_[tail] = buf[r];
        tail = tailp1;
        r++;
        tailp1 = next (tail);
    }
    // Ensure data is copied, then update tail.
    barrier ();
    tail_ = tail;
    return r;
}

} // namespace ucoo

#endif // ucoolib_utils_fifo_tcc
