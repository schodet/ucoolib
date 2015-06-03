#ifndef ucoo_utils_fifo_hh
#define ucoo_utils_fifo_hh
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
#include "ucoo/common.hh"

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

#endif // ucoo_utils_fifo_hh
