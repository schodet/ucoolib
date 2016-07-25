#ifndef ucoo_utils_buffer_hh
#define ucoo_utils_buffer_hh
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

namespace ucoo {

/// Buffer with linear reads and writes.  Looks like a fifo but without thread
/// safety and with poorer performance, but access is linear.
///
/// Warning: no safety net, make sure not to read non present elements or to
/// write more elements than available space.
template<typename T, int max_size>
class Buffer
{
  public:
    /// Constructor, empty buffer.
    Buffer ();
    /// Test whether the buffer is empty.
    bool empty () const;
    /// Test whether the buffer is full (no element can be written, but see
    /// rewind()).
    bool full () const;
    /// Return the number of available elements.
    int size () const;
    /// Return the available space for elements.
    int room () const;
    /// Return pointer to readable elements.
    const T *read () const;
    /// Drop a number of elements after they have been read (or not).
    void drop (int n);
    /// Return pointer to readable elements and drop them (remember: not
    /// thread safe).
    const T *read (int n);
    /// Return pointer to writable space.
    T *write ();
    /// Increase the number of available elements (just written).
    void written (int n);
    /// Return pointer to writable space and increase the number of available
    /// elements (remember: not thread safe).
    T *write (int n);
    /// Move data to begin of buffer to make room after it.
    void rewind ();
  private:
    T data_[max_size];
    T *begin_, *end_;
};

} // namespace ucoo

#include "buffer.tcc"

#endif // ucoo_utils_buffer_hh
