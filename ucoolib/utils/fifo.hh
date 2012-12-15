#ifndef ucoolib_utils_fifo_hh
#define ucoolib_utils_fifo_hh
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
#include "ucoolib/common.hh"

namespace ucoo {

/// First In First Out container.  It is implemented as a circular buffer,
/// providing thread safety if the reader and the writer are in separated
/// threads.
///
/// This is a rather low level container used to implement ucoolib features.
/// Be careful that there is only room for size - 1 elements in the FIFO.
template<typename T, int size>
class Fifo
{
  public:
    /// Constructor, initialise an empty FIFO.
    Fifo ();
    /// Test whether the FIFO is empty.
    bool empty () const { return head_ == tail_; }
    /// Test whether the FIFO is full.
    bool full () const { return head_ == next (tail_); }
    /// Remove an element, do not do that if FIFO is empty!
    T pop ();
    /// Add an element, do not do that if FIFO is full!
    void push (const T &e);
    /// Pop up to COUNT elements and store them in BUF.  Return the number of
    /// read elements.
    int read (T *buf, int count);
    /// Push up to COUNT elements from BUF.  Return the number of written
    /// elements.
    int write (const T *buf, int count);
  private:
    /// Return next index, use unsigned operation for optimisation.
    int next (int index) const
    {
        return (static_cast<unsigned int> (index) + 1) % size;
    }
  private:
    /// Index of the next element to pop, always incremented.
    int_atomic_t head_;
    /// Index of the next free space where to put a pushed element, also
    /// always incremented.
    int_atomic_t tail_;
    /// Memory to store elements.
    T buffer_[size];
};

} // namespace ucoo

#include "fifo.tcc"

#endif // ucoolib_utils_fifo_hh
